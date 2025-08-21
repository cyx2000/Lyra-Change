// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Interaction/Tasks/AbilityTask_WaitForInteractableTargets.h"
#include "AT_WFITargets_Overlap.generated.h"

/**
 * 
 */
UCLASS()
class UAT_WFITargets_Overlap : public UAbilityTask_WaitForInteractableTargets
{
	GENERATED_BODY()

public:

	UAT_WFITargets_Overlap(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Activate() override;

	/** Wait until we trace new set of interactables.  This task automatically loops. */
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAT_WFITargets_Overlap* WaitForInteractableTargets_Overlap(UGameplayAbility* OwningAbility, 
		FInteractionQuery InteractionQuery, 
		FGameplayAbilityTargetingLocationInfo StartLocation, 
		float InteractionScanRange = 150.0f);

private:

	void PerformOverlap();

	UPROPERTY()
	FInteractionQuery InteractionQuery;

	UPROPERTY()
	FGameplayAbilityTargetingLocationInfo StartLocation;

	float InteractionScanRange;
	
};
