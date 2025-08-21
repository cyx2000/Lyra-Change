// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayEffect.h"
#include "XwItemTags.h"
#include "XWorldInventoryItemDefinition.generated.h"

template <typename T> class TSubclassOf;

class UXWorldInventoryItemInstance;
struct FFrame;

//////////////////////////////////////////////////////////////////////

// Represents a fragment of an item definition
UCLASS(MinimalAPI, DefaultToInstanced, EditInlineNew, Abstract)
class UXWorldInventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UXWorldInventoryItemInstance* Instance) const {}
};

//////////////////////////////////////////////////////////////////////

/**
 * UXWorldInventoryItemDefinition
 */
UCLASS(Blueprintable, Const, Abstract)
class UXWorldInventoryItemDefinition : public UObject, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	UXWorldInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Display)
	FInheritedTagContainer ItemTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Display)
	float ItemDefWeight; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display, Instanced)
	TArray<TObjectPtr<UXWorldInventoryItemFragment>> Fragments;

public:

	inline bool IsDefCanStack() const
	{
		return !ItemTags.CombinedTags.HasTagExact(XwItemTags::Item_Equipment);
	}

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	
	XWITEMSYSTEMRUNTIME_API const UXWorldInventoryItemFragment* FindFragmentByClass(TSubclassOf<UXWorldInventoryItemFragment> FragmentClass) const;
};

//@TODO: Make into a subsystem instead?
UCLASS()
class UXWorldInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, meta=(DeterminesOutputType=FragmentClass))
	static const UXWorldInventoryItemFragment* FindItemDefinitionFragment(TSubclassOf<UXWorldInventoryItemDefinition> ItemDef, TSubclassOf<UXWorldInventoryItemFragment> FragmentClass);
};
