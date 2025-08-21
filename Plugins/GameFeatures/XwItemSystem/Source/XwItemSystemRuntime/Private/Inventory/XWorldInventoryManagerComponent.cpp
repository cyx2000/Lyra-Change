// Copyright Epic Games, Inc. All Rights Reserved.

#include "Inventory/XWorldInventoryManagerComponent.h"

#include "Engine/ActorChannel.h"
#include "Engine/World.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Inventory/XWorldInventoryItemDefinition.h"
#include "Inventory/XWorldInventoryItemInstance.h"
#include "Misc/CoreMisc.h"
#include "NativeGameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/Input/IVirtualKeyboardEntry.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(XWorldInventoryManagerComponent)

class FLifetimeProperty;
struct FReplicationFlags;

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_XWorld_Inventory_Message_StackChanged, "XWorld.Inventory.Message.StackChanged");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_XWorld_Inventory_Item_Count, "XWorld.Inventory.Item.Count");

constexpr uint16 MAX_ITEM_STACK_COUNT = 999;
//////////////////////////////////////////////////////////////////////
// FXWorldInventoryEntry

FString FXWorldInventoryEntry::GetDebugString() const
{
	TSubclassOf<UXWorldInventoryItemDefinition> ItemDef;
	if (Instance != nullptr)
	{
		ItemDef = Instance->GetItemDef();
	}

	return FString::Printf(TEXT("%s (%d x %s)"), *GetNameSafe(Instance), StackCount, *GetNameSafe(ItemDef));
}

//////////////////////////////////////////////////////////////////////
// FXWorldInventoryList

void FXWorldInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		FXWorldInventoryEntry& Stack = Entries[Index];
		BroadcastChangeMessage(Stack, /*OldCount=*/ Stack.StackCount, /*NewCount=*/ 0);
		Stack.LastObservedCount = 0;
	}
}

void FXWorldInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		FXWorldInventoryEntry& Stack = Entries[Index];
		BroadcastChangeMessage(Stack, /*OldCount=*/ 0, /*NewCount=*/ Stack.StackCount);
		Stack.LastObservedCount = Stack.StackCount;
	}
}

void FXWorldInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		FXWorldInventoryEntry& Stack = Entries[Index];
		check(Stack.LastObservedCount != INDEX_NONE);
		BroadcastChangeMessage(Stack, /*OldCount=*/ Stack.LastObservedCount, /*NewCount=*/ Stack.StackCount);
		Stack.LastObservedCount = Stack.StackCount;
	}
}

void FXWorldInventoryList::BroadcastChangeMessage(FXWorldInventoryEntry& Entry, int32 OldCount, int32 NewCount)
{
	FXWorldInventoryChangeMessage Message;
	Message.InventoryOwner = OwnerComponent;
	Message.Instance = Entry.Instance;
	Message.NewCount = NewCount;
	Message.Delta = NewCount - OldCount;
	Message.InventoryWeight = CurrentInventoryWeight;
	Message.DefTags = Entry.DefTags;
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSystem.BroadcastMessage(TAG_XWorld_Inventory_Message_StackChanged, Message);
}

UXWorldInventoryItemInstance* FXWorldInventoryList::AddEntry(TSubclassOf<UXWorldInventoryItemDefinition> ItemDef, int32 StackCount)
{
	UXWorldInventoryItemInstance* Result = nullptr;

	check(ItemDef != nullptr);
 	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	const UXWorldInventoryItemDefinition* EntryDefObj = GetDefault<UXWorldInventoryItemDefinition>(ItemDef);

	FXWorldInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<UXWorldInventoryItemInstance>(OwnerComponent->GetOwner());  //@TODO: Using the actor instead of component as the outer due to UE-127172
	NewEntry.Instance->SetItemDef(ItemDef);

	NewEntry.EntryID = IDCounter;
	// EntryDefObj->GetOwnedGameplayTags(NewEntry.DefTags);
	EntryDefObj->ItemTags.ApplyTo(NewEntry.DefTags);
	// NewEntry.EntryWeight = EntryDefObj->ItemDefWeight * StackCount;

	CurrentInventoryWeight += (EntryDefObj->ItemDefWeight * StackCount);

	NewEntry.Instance->AddStatTagStack(TAG_XWorld_Inventory_Item_Count, StackCount);

	for (UXWorldInventoryItemFragment* Fragment : EntryDefObj->Fragments)
	{
		if (Fragment != nullptr)
		{
			Fragment->OnInstanceCreated(NewEntry.Instance);
		}
	}
	NewEntry.StackCount = StackCount;
	Result = NewEntry.Instance;

	//const UXWorldInventoryItemDefinition* ItemCDO = GetDefault<UXWorldInventoryItemDefinition>(ItemDef);
	MarkItemDirty(NewEntry);

	if(!IsRunningDedicatedServer())
	{
		BroadcastChangeMessage(NewEntry, 0, NewEntry.StackCount);
	}

	return Result;
}

void FXWorldInventoryList::AddEntry(UXWorldInventoryItemInstance* Instance)
{
	unimplemented();
}

void FXWorldInventoryList::ChangeEntryCount(FXWorldInventoryEntry* InEntry, int32 InStackCount)
{
	InEntry->StackCount += InStackCount;

	const UXWorldInventoryItemDefinition* EntryDefObj = GetDefault<UXWorldInventoryItemDefinition>(InEntry->Instance->GetItemDef());
	
	CurrentInventoryWeight += (InStackCount * EntryDefObj->ItemDefWeight);

	MarkItemDirty(*InEntry);
}

void FXWorldInventoryList::RemoveEntry(UXWorldInventoryItemInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FXWorldInventoryEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			const UXWorldInventoryItemDefinition* EntryDefObj = GetDefault<UXWorldInventoryItemDefinition>(Entry.Instance->GetItemDef());
			CurrentInventoryWeight -= (Entry.StackCount * EntryDefObj->ItemDefWeight);
			EntryIt.RemoveCurrent();
			MarkArrayDirty();

			if(!IsRunningDedicatedServer())
			{
				BroadcastChangeMessage(Entry, Entry.StackCount, 0);
			}
		}
	}
}

TArray<UXWorldInventoryItemInstance*> FXWorldInventoryList::GetAllItems() const
{
	TArray<UXWorldInventoryItemInstance*> Results;
	Results.Reserve(Entries.Num());
	for (const FXWorldInventoryEntry& Entry : Entries)
	{
		if (Entry.Instance != nullptr) //@TODO: Would prefer to not deal with this here and hide it further?
		{
			Results.Emplace(Entry.Instance);
		}
	}
	return Results;
}

TArray<UXWorldInventoryItemInstance*> FXWorldInventoryList::GetTagsItems(const FGameplayTagContainer& DefTags) const
{
	TArray<UXWorldInventoryItemInstance*> Results;
	Results.Reserve(Entries.Num());
	for (const FXWorldInventoryEntry& Entry : Entries)
	{
		if (Entry.DefTags.HasAll(DefTags)) 
		{
			Results.Emplace(Entry.Instance);
		}
	}
	Results.Shrink();
	return Results;
}

//////////////////////////////////////////////////////////////////////
// UXWorldInventoryManagerComponent

UXWorldInventoryManagerComponent::UXWorldInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryList(this)
{
	SetIsReplicatedByDefault(true);
}

void UXWorldInventoryManagerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
}

bool UXWorldInventoryManagerComponent::CanAddItemDefinition(TSubclassOf<UXWorldInventoryItemDefinition> ItemDef, int32 StackCount)
{
	//@TODO: Add support for stack limit / uniqueness checks / etc...
	return true;
}

UXWorldInventoryItemInstance* UXWorldInventoryManagerComponent::AddItemDefinition(TSubclassOf<UXWorldInventoryItemDefinition> ItemDef, int32 StackCount)
{
	UXWorldInventoryItemInstance* Result = nullptr;
	if (ItemDef != nullptr)
	{
		Result = InventoryList.AddEntry(ItemDef, StackCount);
		
		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && Result)
		{
			AddReplicatedSubObject(Result);
		}
	}
	return Result;
}

void UXWorldInventoryManagerComponent::AddItemInstance(UXWorldInventoryItemInstance* ItemInstance)
{
	InventoryList.AddEntry(ItemInstance);
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && ItemInstance)
	{
		AddReplicatedSubObject(ItemInstance);
	}
}

void UXWorldInventoryManagerComponent::RemoveItemInstance(UXWorldInventoryItemInstance* ItemInstance)
{
	InventoryList.RemoveEntry(ItemInstance);

	if (ItemInstance && IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(ItemInstance);
	}
}

TArray<UXWorldInventoryItemInstance*> UXWorldInventoryManagerComponent::GetAllItems() const
{
	return InventoryList.GetAllItems();
}

TArray<UXWorldInventoryItemInstance*> UXWorldInventoryManagerComponent::GetHasTagsItems(const FGameplayTagContainer& InTags) const
{
	return InventoryList.GetTagsItems(InTags);
}

UXWorldInventoryItemInstance* UXWorldInventoryManagerComponent::FindFirstItemStackByDefinition(TSubclassOf<UXWorldInventoryItemDefinition> ItemDef) const
{
	for (const FXWorldInventoryEntry& Entry : InventoryList.Entries)
	{
		UXWorldInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

UXWorldInventoryItemInstance* UXWorldInventoryManagerComponent::FindItemInstanceByEntryIDAndTag(int32 InEntryID, const FGameplayTag& InTypeTag) const
{
	for (const FXWorldInventoryEntry& Entry : InventoryList.Entries)
	{
		if (Entry.EntryID == InEntryID && Entry.DefTags.HasTagExact(InTypeTag) )
		{
			return Entry.Instance;
		}
	}
	return nullptr;
}

FXWorldInventoryEntry* UXWorldInventoryManagerComponent::FindItemEntryByDefinition(TSubclassOf<UXWorldInventoryItemDefinition> InItemDef)
{
	for (FXWorldInventoryEntry& Entry : InventoryList.Entries)
	{
		if(Entry.Instance->GetItemDef() == InItemDef)
		{
			return &Entry;
		}
	}
	return nullptr;
}

int32 UXWorldInventoryManagerComponent::GetTotalItemCountByDefinition(TSubclassOf<UXWorldInventoryItemDefinition> ItemDef) const
{
	int32 TotalCount = 0;
	for (const FXWorldInventoryEntry& Entry : InventoryList.Entries)
	{
		UXWorldInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				++TotalCount;
			}
		}
	}

	return TotalCount;
}

bool UXWorldInventoryManagerComponent::ConsumeItemsByDefinition(TSubclassOf<UXWorldInventoryItemDefinition> ItemDef, int32 NumToConsume)
{
	AActor* OwningActor = GetOwner();
	if (!OwningActor || !OwningActor->HasAuthority())
	{
		return false;
	}

	//@TODO: N squared right now as there's no acceleration structure
	int32 TotalConsumed = 0;
	while (TotalConsumed < NumToConsume)
	{
		if (UXWorldInventoryItemInstance* Instance = UXWorldInventoryManagerComponent::FindFirstItemStackByDefinition(ItemDef))
		{
			InventoryList.RemoveEntry(Instance);
			++TotalConsumed;
		}
		else
		{
			return false;
		}
	}

	return TotalConsumed == NumToConsume;
}

void UXWorldInventoryManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing UXWorldInventoryItemInstance
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FXWorldInventoryEntry& Entry : InventoryList.Entries)
		{
			UXWorldInventoryItemInstance* Instance = Entry.Instance;

			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

bool UXWorldInventoryManagerComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FXWorldInventoryEntry& Entry : InventoryList.Entries)
	{
		UXWorldInventoryItemInstance* Instance = Entry.Instance;

		if (Instance && IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

//////////////////////////////////////////////////////////////////////
//

// UCLASS(Abstract)
// class UXWorldInventoryFilter : public UObject
// {
// public:
// 	virtual bool PassesFilter(UXWorldInventoryItemInstance* Instance) const { return true; }
// };

// UCLASS()
// class UXWorldInventoryFilter_HasTag : public UXWorldInventoryFilter
// {
// public:
// 	virtual bool PassesFilter(UXWorldInventoryItemInstance* Instance) const { return true; }
// };

