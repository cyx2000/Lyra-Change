// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "GenericTeamAgentInterface.h"
#include "XwAISpawner.generated.h"

class ULyraPawnData;
class ULyraExperienceDefinition;
class AAIController;
class AXwPickupableActor;
class ULyraAbilitySet;
class ATargetPoint;

USTRUCT(BlueprintType)
struct FAISpawnData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AIPawn)
	TSoftObjectPtr<ULyraPawnData> AIPawnData = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AIPawn)
	TArray<TSoftObjectPtr<ULyraAbilitySet>> AbilitySets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AIPawn)
	TObjectPtr<ATargetPoint> SpawnTargetActor;
};

struct FAISpawnDataList
{

	FAISpawnDataList()
		:AIPawnData(nullptr),
		AbilitySets(nullptr),
		TargetTransform()
	{

	}
	TSoftObjectPtr<ULyraPawnData> AIPawnData;

	const TArray<TSoftObjectPtr<ULyraAbilitySet>>* AbilitySets;

	FTransform TargetTransform;

};

UCLASS(MinimalAPI, Abstract)
class AXwAISpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	XPROWORLDRUNTIME_API AXwAISpawner();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AIPawn)
	TArray<FAISpawnData> AIPawnDataList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FGenericTeamId TeamID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay)
	float LocationBound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay)
	float SpawnTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay)
	int32 EveryMaxSpawnCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay)
	TSubclassOf<AXwPickupableActor> PickupActor;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category=Gameplay)
	TSubclassOf<AAIController> BotControllerClass;

	/** #todo find out how to pool the AIControllers. At the moment the controllers are destroyed with APawn::DetachFromControllerPendingDestroy() **/
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<AAIController>> SpawnedBotList;

	/** Always creates a single bot */
	virtual void SpawnOneBot(const ULyraPawnData* WantData, const FTransform& InTransform, 
		const TArray<TSoftObjectPtr<ULyraAbilitySet>>* InAbilitySets);

	/** Deletes the last created bot if possible */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Gameplay)
	virtual void RemoveOneBot();

	/** Spawns bots up to NumBotsToCreate */
	UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, Category=Gameplay)
	void ServerCreateBots();

	// Called when the death sequence for the character has completed
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnAIDestroyed"), BlueprintAuthorityOnly)
	void K2_OnAIDestroyed(AActor* InDestroyedActor);

	UFUNCTION()
	void OnSpawnedPawnDestroyed(AActor* InDestroyedActor);

	void OnBeforePawnDestroy(AActor* InDestroyedPawn);
	
	UFUNCTION()
	void HandleSpawn();

private:
	void OnExperienceLoaded(const ULyraExperienceDefinition* Experience);

	TArray<FAISpawnDataList> SpawnList;

	FTimerHandle SpawnTimerHandle;
};
