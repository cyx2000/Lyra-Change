// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "GenericTeamAgentInterface.h"
#include "XwAISpawner.generated.h"

class ULyraPawnData;
class ULyraExperienceDefinition;
class AAIController;
class AXwPickupableActor;

UCLASS(MinimalAPI, Abstract)
class AXwAISpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	XPROWORLDRUNTIME_API AXwAISpawner();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pawn)
	TMap<TSoftObjectPtr<ULyraPawnData>, uint8> PawnData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FGenericTeamId TeamID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay)
	float LocationBound;

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
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Gameplay)
	virtual void SpawnOneBot(const ULyraPawnData* WantData, const FVector& Location);

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

private:
	void OnExperienceLoaded(const ULyraExperienceDefinition* Experience);

};
