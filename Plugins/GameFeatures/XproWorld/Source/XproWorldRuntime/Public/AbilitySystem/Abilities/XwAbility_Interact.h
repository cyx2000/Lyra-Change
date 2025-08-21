// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "Interaction/IInteractableTarget.h"
#include "XwAbility_Interact.generated.h"

struct FOverlapResult;
/**
 * 
 */
UCLASS(MinimalAPI, Abstract)
class UXwAbility_Interact : public ULyraGameplayAbility
{
	GENERATED_BODY()

public:
	XPROWORLDRUNTIME_API UXwAbility_Interact(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle InHandle, 
		const FGameplayAbilityActorInfo* InActorInfo, 
		const FGameplayAbilityActivationInfo InActivationInfo, 
		const FGameplayEventData* InTriggerEventData) override;

protected:

	TMap<FObjectKey, FGameplayAbilitySpecHandle> InteractionAbilityCache;

	TArray<TScriptInterface<IInteractableTarget>> PickupableInteractableTargets;

	void GetInteractableTargetsFromOverlapResults(const TArray<FOverlapResult>& OverlapResults, TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets);
	
	void TriggerInteractionFromOverlap();


private:

	void GetInteractableTargetsFromActor(AActor* InActor, TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets);

	void GetInteractableTargetsFromComponent(UPrimitiveComponent* InComponent, TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets);

	void GetInteractableTargetsFromActorOrComponent(AActor* InActor, UPrimitiveComponent* InComponent, TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets);

	void MakeActivateData(const FInteractionOption& InInteractionOption, FGameplayEventData& InPayload, FGameplayAbilityActorInfo& InActorInfo);

};
