// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/XwAISpawner.h"
// #include "Game/XpWorldObjPoolSubsystem.h"
#include "GameFramework/Pawn.h"
#include "Character/LyraPawnExtensionComponent.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Teams/LyraTeamSubsystem.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/LyraAbilitySet.h"
#include "GameModes/LyraExperienceManagerComponent.h"
#include "DataRegistrySubsystem.h"
#include "Game/XpWorldItem.h"
#include "Inventory/XWorldInventoryItemDefinition.h"
#include "Inventory/XwPickupableActor.h"
#include "Inventory/XWInventoryFragment_QuickBarIcon.h"
#include "AI/XwAICharacter.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "Character/LyraPawnData.h"
#include "Engine/TargetPoint.h"
#include "Components/SplineComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(XwAISpawner)

const FName AXwAISpawner::DRAIDataName = FName(TEXT("DR_AIData"));
const FName AXwAISpawner::AIEnemyName = FName(TEXT("AIEnemy"));
const FName AXwAISpawner::EnemyPawnData = FName(TEXT("AIPawnData"));
// Sets default values
AXwAISpawner::AXwAISpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SpawnList.Empty();
	SpawnedBotList.Empty();
	EveryMaxSpawnCount = 1;
}

// Called when the game starts or when spawned
void AXwAISpawner::BeginPlay()
{
	Super::BeginPlay();

	if(SpawnedBotList.Num() > 0)
	{
		return;
	}
	
	// Listen for the experience load to complete
	AGameStateBase* GameState = GetWorld()->GetGameState();
	ULyraExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<ULyraExperienceManagerComponent>();
	check(ExperienceComponent);
	ExperienceComponent->CallOrRegister_OnExperienceLoaded_LowPriority(FOnLyraExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

void AXwAISpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	
	Super::EndPlay(EndPlayReason);
}


void AXwAISpawner::ServerCreateBots_Implementation()
{
	// if (BotControllerClass == nullptr)
	// {
	// 	return;
	// }

	UWorld* World = GetWorld();  //world用各种方式也都可以

	{
		FTransform SpawnerTransform = GetActorTransform();

		const bool bNeedRandomLocation = (LocationBound > 50.f);
		
		UDataRegistrySubsystem* DRsystem = UDataRegistrySubsystem::Get();

		check(DRsystem);

		FDataRegistryId WantID(AXwAISpawner::DRAIDataName, AIEnemyName);

		const FXpWorldInstanceBag* EnemyData = DRsystem->GetCachedItem<FXpWorldInstanceBag>(WantID);
		check(EnemyData);

		for(const auto& SpawnData: AIPawnDataList)
		{
			FSoftObjectPath OutAIPawnData;
			if(auto TagGASPath = EnemyData->XpBag.GetValueSoftPath(SpawnData.EnemyData); TagGASPath.IsValid())
			{
				OutAIPawnData = TagGASPath.GetValue();
			}
			else 
			{
				WantID.ItemName = SpawnData.EnemyData;
				const FXpWorldInstanceBag* InstanceEnemyData = DRsystem->GetCachedItem<FXpWorldInstanceBag>(WantID);
				check(InstanceEnemyData);
				const auto& SpawnPawnData = InstanceEnemyData->XpBag.GetValueName(EnemyPawnData).GetValue();

				OutAIPawnData = EnemyData->XpBag.GetValueSoftPath(SpawnPawnData).GetValue();
			}
			
			OutAIPawnData.TryLoad();

			const bool bHasTargetLocation = (SpawnData.SpawnTargetActor != nullptr);

			FAISpawnDataList& AISpawnData = SpawnList.Emplace_GetRef();

			AISpawnData.AIPawnData = OutAIPawnData;

			AISpawnData.TargetTransform = bHasTargetLocation ? SpawnData.SpawnTargetActor->GetActorTransform() : SpawnerTransform;
			if(SpawnData.PatrolSplinePointInputKey >= 0.f)
			{
				USplineComponent* DefaultSpline = FindComponentByClass<USplineComponent>();
				check(DefaultSpline);

				AISpawnData.TargetTransform.SetLocation(DefaultSpline->GetLocationAtSplineInputKey(SpawnData.PatrolSplinePointInputKey, ESplineCoordinateSpace::World));
			}
			else if(bNeedRandomLocation && !bHasTargetLocation)
			{
				FNavLocation RandomPoint(SpawnerTransform.GetLocation());
				UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
				if (NavSys)
				{
					NavSys->GetRandomReachablePointInRadius(SpawnerTransform.GetLocation(), 
					LocationBound, 
					RandomPoint, 
					NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate));

					AISpawnData.TargetTransform.SetLocation(RandomPoint.Location);
				}
			}

			AISpawnData.InEnemyName = &SpawnData.EnemyData;

		}
	}

	FTimerDelegate Delegate;

	Delegate.BindUObject(this, &ThisClass::HandleSpawn);

	if(SpawnTime > 0.f)
	{
		World->GetTimerManager().SetTimer(SpawnTimerHandle, Delegate, SpawnTime, true);
	}
	else 
	{
		World->GetTimerManager().SetTimerForNextTick(Delegate);
	}

}

void AXwAISpawner::SpawnOneBot(const ULyraPawnData* WantData, const FTransform& InTransform, 
	const FName* InEnemyNameData)
{
	UWorld* World = GetWorld();  //world用各种方式也都可以

	check(World);

	AXwAICharacter* NewPawn = nullptr;

	// const bool bHasAIController = BotControllerClass != nullptr;

	{
		FActorSpawnParameters ActorSpawnParams;
		// ActorSpawnParams.Owner = this;
		ActorSpawnParams.ObjectFlags |= RF_Transient;	// We never want to save spawned AI pawns into a map
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		// defer spawning the pawn to setup the AIController, else it spawns the default controller on spawn if set to spawn AI on spawn
		ActorSpawnParams.bDeferConstruction = true;

		TSubclassOf<AActor> WantClass = WantData->PawnClass;

		// UXpWorldObjPoolSubsystem* PoolSubsystem = World->GetSubsystem<UXpWorldObjPoolSubsystem>();

		// NewPawn = Cast<APawn>(PoolSubsystem->CreateOrGetActorInternal(WantClass, InTransform, ActorSpawnParams));
		NewPawn = Cast<AXwAICharacter>(World->SpawnActor(WantClass, &InTransform, ActorSpawnParams));

	}

	if(BotControllerClass != nullptr)
	{
		NewPawn->AIControllerClass = BotControllerClass;
	}
	{
		if (ULyraPawnExtensionComponent* PawnExtComp = ULyraPawnExtensionComponent::FindPawnExtensionComponent(NewPawn))
		{
			PawnExtComp->SetPawnData(WantData);
		}
		NewPawn->FinishSpawning(InTransform);
	}
	
	bool bWantsPlayerState = true;
	if (AAIController* AIController = Cast<AAIController>(NewPawn->Controller))
	{
		bWantsPlayerState = AIController->bWantsPlayerState;
		AIController->SetOwner(this);
	}
	
	if(InEnemyNameData)
	{
		NewPawn->EnemyName = *InEnemyNameData;
	}

	if (ULyraPawnExtensionComponent* PawnExtComp = ULyraPawnExtensionComponent::FindPawnExtensionComponent(NewPawn))
	{
		AActor* AbilityOwner = bWantsPlayerState ? NewPawn->GetPlayerState() : Cast<AActor>(NewPawn);
		
		if (ULyraAbilitySystemComponent* AbilitySystemComponent = Cast<ULyraAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(AbilityOwner)))
		{
			PawnExtComp->InitializeAbilitySystem(AbilitySystemComponent, AbilityOwner);
		}
	}

	check(NewPawn);

	NewPawn->OnDestroyed.AddDynamic(this, &ThisClass::OnSpawnedPawnDestroyed);

	// Cast<AXwAICharacter>(NewPawn)->BeforeDestory = &ThisClass::OnBeforePawnDestroy;

	if (ULyraTeamSubsystem* TeamSubsystem = UWorld::GetSubsystem<ULyraTeamSubsystem>(World))
	{
		TeamSubsystem->ChangeTeamForActor(NewPawn->Controller, TeamID);
	}

	SpawnedBotList.Emplace(Cast<AAIController>(NewPawn->Controller));

}

void AXwAISpawner::RemoveOneBot()
{

}

void AXwAISpawner::OnExperienceLoaded(const ULyraExperienceDefinition* Experience)
{
#if WITH_SERVER_CODE
	if(HasAuthority())
	{
		// ServerCreateBots();
		FTimerDelegate Delegate;

		Delegate.BindUObject(this, &ThisClass::ServerCreateBots);

		FTimerManager& TimerManager = GetWorld()->GetTimerManager();

		TimerManager.SetTimerForNextTick(Delegate);
	}
#endif
}

void AXwAISpawner::OnBeforePawnDestroy(AActor* InDestroyedPawn)
{

}

void AXwAISpawner::OnSpawnedPawnDestroyed(AActor* InDestroyedActor)
{

	InDestroyedActor->OnDestroyed.RemoveAll(this);

	K2_OnAIDestroyed(InDestroyedActor);

	if(Tags.IsEmpty())
	{
		return;
	}
	
	static FDataRegistryId RegistryId(TEXT("DR_ContryItems"), NAME_None);
	static TMap<FString, uint8> CurrentFalledItems{};

	UDataRegistrySubsystem* DRSub = UDataRegistrySubsystem::Get();

	check(DRSub);

	UWorld* World = GetWorld();

	for(const auto& CountryName: Tags)
	{
		check(CountryName != NAME_None);

		RegistryId.ItemName = CountryName;

		const FXpWorldConutryItems* CurrentItems = DRSub->GetCachedItem<FXpWorldConutryItems>(RegistryId);

		check(CurrentItems);

		int32 ItemsNum = CurrentItems->Items.Num();
		int8 WantIndex{INDEX_NONE};
		if(ItemsNum == 0)
		{
			continue;
		}
		else 
		{
			for(int32 i = 0; i < CurrentItems->NumToSpawn; ++i)
			{
				int32 TotalWeight{};	
				
				for(const auto& CurrentItem: CurrentItems->Items)
				{
					const auto& ItemName = CurrentItem.ItemDef->GetName();

					int32 WantCount{CurrentItem.Count};
					if(auto AlreadyFallCount = CurrentFalledItems.Find(ItemName))
					{
						if(*AlreadyFallCount < WantCount)
						{
							WantCount -= *AlreadyFallCount;
						}
						else 
						{
							continue;
						}
					}
					
					TotalWeight += (WantCount * CurrentItem.Weight);
				}

				if(TotalWeight > 0)
				{
					int32 RandWeight = FMath::RandRange(1, TotalWeight);
					
					if(RandWeight > (TotalWeight * CurrentItems->NonePercent))
					{
						int32 AddWeight{};
						for(int32 i = 0; i < ItemsNum; ++i)
						{
							const auto& CurrentItem = CurrentItems->Items[i];
							const auto& ItemName = CurrentItem.ItemDef->GetName();

							int32 WantCount{CurrentItem.Count};

							auto AlreadyFallCount = CurrentFalledItems.Find(ItemName);
							if(AlreadyFallCount)
							{
								if(*AlreadyFallCount < WantCount)
								{
									WantCount -= *AlreadyFallCount;
								}
								else 
								{
									continue;
								}
							}
							AddWeight += (WantCount * CurrentItem.Weight);

							if(AddWeight >= TotalWeight)
							{
								if(AlreadyFallCount)
								{
									*AlreadyFallCount += 1;
								}
								else 
								{
									CurrentFalledItems.Emplace(ItemName, 1);
								}
								WantIndex = i;
								break;
							}
						}
					}
				}
			}
		}

		if(WantIndex != INDEX_NONE)
		{
			const TSubclassOf<UXWorldInventoryItemDefinition> WantItem = CurrentItems->Items[WantIndex].ItemDef;

			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.Owner = this;
			ActorSpawnParams.ObjectFlags |= RF_Transient;	// We never want to save spawned AI pawns into a map
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			// defer spawning the pawn
			ActorSpawnParams.bDeferConstruction = true;

			AXwPickupableActor* SpawnPickupActor = World->SpawnActor<AXwPickupableActor>(PickupActor, 
				ActorSpawnParams);

			TSoftObjectPtr<UStaticMesh> SoftMesh = Cast<UXWInventoryFragment_QuickBarIcon>(
				GetDefault<UXWorldInventoryItemDefinition>(WantItem)->FindFragmentByClass(
					UXWInventoryFragment_QuickBarIcon::StaticClass()))->DisplayMesh;
			UStaticMesh* WantMesh = SoftMesh.Get();
			if(WantMesh == nullptr)
			{
				WantMesh = SoftMesh.LoadSynchronous();
			}
			SpawnPickupActor->AddPickupItemByDef(WantItem);
			SpawnPickupActor->DisplayMesh = WantMesh;

			const FRotator& Rotation = InDestroyedActor->GetActorRotation();
			const FVector& Location = InDestroyedActor->GetActorLocation();

			SpawnPickupActor->FinishSpawning(FTransform(Rotation, Location, FVector::OneVector));
		}
	}

}


void AXwAISpawner::HandleSpawn()
{
	{
		const auto& CurrentNum = SpawnList.Num();

		check(EveryMaxSpawnCount > 0);

		uint8 CanSpawnCount = CurrentNum > EveryMaxSpawnCount ? static_cast<uint8>(EveryMaxSpawnCount) : static_cast<uint8>(CurrentNum);

		for (auto It = SpawnList.CreateIterator(); It; ++It)
		{
			if(CanSpawnCount == 0)
			{
				break;
			}

			CanSpawnCount -= 1;

			SpawnOneBot(It->AIPawnData.Get(), It->TargetTransform, It->InEnemyName);
			
			It.RemoveCurrent();

		}

	}

	if (SpawnList.IsEmpty()) 
	{
		if(SpawnTime > 0.f)
		{
			FTimerManager& TimerManager = GetWorld()->GetTimerManager();
			TimerManager.ClearTimer(SpawnTimerHandle);
		}
	}
	else
	{
		if(SpawnTime <= 0.f)
		{
			FTimerDelegate Delegate;

			Delegate.BindUObject(this, &ThisClass::HandleSpawn);

			FTimerManager& TimerManager = GetWorld()->GetTimerManager();

			TimerManager.SetTimerForNextTick(Delegate);
		}
	}
}
