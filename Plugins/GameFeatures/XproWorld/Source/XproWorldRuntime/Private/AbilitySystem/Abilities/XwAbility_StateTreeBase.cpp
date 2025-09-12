// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/XwAbility_StateTreeBase.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "StateTree/Tasks/XwSTT_ActivateAbilityByEvent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(XwAbility_StateTreeBase)

void UXwAbility_StateTreeBase::EndStateTreeTaskAbilityLocally(bool bFinishedTask)
{
    ensure(CurrentActorInfo);

    for(const auto& TiggerEvent: AbilityTriggers)
    {
        switch (TiggerEvent.TriggerSource) {
            case EGameplayAbilityTriggerSource::GameplayEvent:
            {
                BroadcastTaskAbilityFinished(TiggerEvent.TriggerTag, bFinishedTask);
            }
            break;
            case EGameplayAbilityTriggerSource::OwnedTagPresent:
            case EGameplayAbilityTriggerSource::OwnedTagAdded:
            default:
            break;
        }
    }

	bool bReplicateEndAbility = false;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UXwAbility_StateTreeBase::BroadcastTaskAbilityFinished(const FGameplayTag& InEventTag, const bool bFinished)
{
    UWorld* World = GetWorld();

    if (UGameplayMessageSubsystem::HasInstance(World))
    {
        UGameplayMessageSubsystem& Router = UGameplayMessageSubsystem::Get(World);

        FXwST_StateTreeTaskData TaskState(bFinished);

        Router.BroadcastMessageInternal(InEventTag, FXwST_StateTreeTaskData::StaticStruct(), &TaskState);
    }
}
