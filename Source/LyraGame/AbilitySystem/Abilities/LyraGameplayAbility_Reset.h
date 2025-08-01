// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"

#include "LyraGameplayAbility_Reset.generated.h"

#define UE_API LYRAGAME_API

class AActor;
class UObject;
struct FGameplayAbilityActorInfo;
struct FGameplayEventData;

/**
 * ULyraGameplayAbility_Reset
 *
 *	Gameplay ability used for handling quickly resetting the player back to initial spawn state.
 *	Ability is activated automatically via the "GameplayEvent.RequestReset" ability trigger tag (server only).
 */
UCLASS(MinimalAPI)
class ULyraGameplayAbility_Reset : public ULyraGameplayAbility
{
	GENERATED_BODY()

public:
	UE_API ULyraGameplayAbility_Reset(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	
	UE_API virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};



USTRUCT(BlueprintType)
struct FLyraPlayerResetMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> OwnerPlayerState = nullptr;
};

#undef UE_API
