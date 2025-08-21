// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/XwAbility_Interact.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"
#include "Inventory/IXWPickupable.h"
#include "Interaction/IInteractableTarget.h"
#include "Interaction/InteractionOption.h"
#include "Interaction/InteractionQuery.h"
#include "Engine/OverlapResult.h"
#include "Interaction/InteractionStatics.h"
#include "Misc/AssertionMacros.h"
#include "NativeGameplayTags.h"
#include "Physics/LyraCollisionChannels.h"
#include "Serialization/MemoryLayout.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(XwAbility_Interact)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Ability_Interaction_Activate, "XwAbility.Interaction.Activate");

UXwAbility_Interact::UXwAbility_Interact(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
    ActivationPolicy = ELyraAbilityActivationPolicy::OnInputTriggered;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UXwAbility_Interact::ActivateAbility( 
    const FGameplayAbilitySpecHandle InHandle, 
    const FGameplayAbilityActorInfo* InActorInfo, 
    const FGameplayAbilityActivationInfo InActivationInfo, 
    const FGameplayEventData* InTriggerEventData)
{
    Super::ActivateAbility(InHandle, InActorInfo, InActivationInfo, InTriggerEventData);

    TriggerInteractionFromOverlap();

}

void UXwAbility_Interact::TriggerInteractionFromOverlap()
{
    UWorld* World = GetWorld();
	AActor* ActorOwner = GetAvatarActorFromActorInfo();
	
	if (World && ActorOwner)
	{
		FCollisionQueryParams Params(SCENE_QUERY_STAT(UXwAbility_Interact), false);
        TArray<FOverlapResult> OverlapResults;
		World->OverlapMultiByChannel(OUT OverlapResults, ActorOwner->GetActorLocation(), FQuat::Identity, Lyra_TraceChannel_Interaction, FCollisionShape::MakeSphere(150.f), Params);

		if (OverlapResults.Num() > 0)
		{
            TArray<TScriptInterface<IInteractableTarget>> InteractableTargets;
            GetInteractableTargetsFromOverlapResults(OverlapResults, InteractableTargets);
            TScriptInterface<IInteractableTarget> CurrentTarget;
            if (PickupableInteractableTargets.Num() > 0) 
            {
                check((PickupableInteractableTargets.Num() == 1));
                CurrentTarget = PickupableInteractableTargets.Pop();
            }
            else 
            {
                CurrentTarget = InteractableTargets[0];
            }

            FInteractionQuery InteractionQuery;
			InteractionQuery.RequestingAvatar = ActorOwner;
			InteractionQuery.RequestingController = Cast<AController>(ActorOwner->GetOwner());

            TArray<FInteractionOption> Options;
			FInteractionOptionBuilder InteractionBuilder(CurrentTarget, Options);
			CurrentTarget->GatherInteractionOptions(InteractionQuery, InteractionBuilder);

            for (FInteractionOption& Option : Options)
			{

                FGameplayAbilitySpec* InteractionAbilitySpec = nullptr;

                // if there is a handle an a target ability system, we're triggering the ability on the target.
                if (Option.TargetAbilitySystem && Option.TargetInteractionAbilityHandle.IsValid())
                {
                    // Find the spec
                    InteractionAbilitySpec = Option.TargetAbilitySystem->FindAbilitySpecFromHandle(Option.TargetInteractionAbilityHandle);
                }
				else if (Option.InteractionAbilityToGrant)
				{
				
                    InteractionAbilitySpec = GetAbilitySystemComponentFromActorInfo()->FindAbilitySpecFromClass(Option.InteractionAbilityToGrant);
                    if (InteractionAbilitySpec)
                    {
                        // update the option
                        Option.TargetAbilitySystem = GetAbilitySystemComponentFromActorInfo();
                        Option.TargetInteractionAbilityHandle = InteractionAbilitySpec->Handle;
                    }
					
				}

                FGameplayEventData Payload;

                FGameplayAbilityActorInfo ActorInfo;

                MakeActivateData(Option, Payload, ActorInfo);

                if (InteractionAbilitySpec)
                {                    

                    Option.TargetAbilitySystem->TriggerAbilityFromGameplayEvent(
                        Option.TargetInteractionAbilityHandle,
                        &ActorInfo,
                        TAG_Ability_Interaction_Activate,
                        &Payload,
                        *Option.TargetAbilitySystem
                    );
                }
                else if(HasAuthority(&CurrentActivationInfo))
                {
                    FGameplayAbilitySpec Spec(Option.InteractionAbilityToGrant, 1, INDEX_NONE, this);
                    FGameplayAbilitySpecHandle Handle = GetAbilitySystemComponentFromActorInfo()->GiveAbilityAndActivateOnce(Spec, &Payload);
                }
			}
        }
    }

}


void UXwAbility_Interact::GetInteractableTargetsFromOverlapResults(const TArray<FOverlapResult>& InOverlapResults, TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets)
{
    for (const FOverlapResult& Overlap : InOverlapResults)
	{
        AActor* OverlapActor = Overlap.GetActor();

        TScriptInterface<IXWPickupable> PickupActor(OverlapActor);

        if(PickupActor)
        {
            GetInteractableTargetsFromActor(OverlapActor, PickupableInteractableTargets);
            return;
        }

        UPrimitiveComponent* OverlapComp = Overlap.GetComponent();
        TScriptInterface<IXWPickupable> PickupComp(OverlapComp);
        
        if(PickupComp)
        {
            GetInteractableTargetsFromComponent(OverlapComp, PickupableInteractableTargets);
            return;
        }

        GetInteractableTargetsFromActorOrComponent(OverlapActor, OverlapComp, OutInteractableTargets);

	}
}

void UXwAbility_Interact::GetInteractableTargetsFromActor(AActor* InActor, TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets)
{
    TScriptInterface<IInteractableTarget> InteractableActor(InActor);
	if (InteractableActor)
	{
		OutInteractableTargets.AddUnique(InteractableActor);
	}

}

void UXwAbility_Interact::GetInteractableTargetsFromComponent(UPrimitiveComponent* InComponent, TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets)
{
    TScriptInterface<IInteractableTarget> InteractableComponent(InComponent);
	if (InteractableComponent)
	{
		OutInteractableTargets.AddUnique(InteractableComponent);
	}

    unimplemented();
}

void UXwAbility_Interact::GetInteractableTargetsFromActorOrComponent(AActor* InActor, UPrimitiveComponent* InComponent, TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets)
{

    TScriptInterface<IInteractableTarget> InteractableActor(InActor);
	if (InteractableActor)
	{
		OutInteractableTargets.AddUnique(InteractableActor);
        return;
	}
    TScriptInterface<IInteractableTarget> InteractableComponent(InComponent);
	if (InteractableComponent)
	{
		OutInteractableTargets.AddUnique(InteractableComponent);
        return;
	}
    unimplemented();
}


void UXwAbility_Interact::MakeActivateData(const FInteractionOption& InInteractionOption, FGameplayEventData& InPayload, FGameplayAbilityActorInfo& InActorInfo)
{
    AActor* Instigator = GetAvatarActorFromActorInfo();
    AActor* InteractableTargetActor = UInteractionStatics::GetActorFromInteractableTarget(InInteractionOption.InteractableTarget);

    // Allow the target to customize the event data we're about to pass in, in case the ability needs custom data
    // that only the actor knows.

    InPayload.EventTag = TAG_Ability_Interaction_Activate;
    InPayload.Instigator = Instigator;
    InPayload.Target = InteractableTargetActor;

    // If needed we allow the interactable target to manipulate the event data so that for example, a button on the wall
    // may want to specify a door actor to execute the ability on, so it might choose to override Target to be the
    // door actor.
    InInteractionOption.InteractableTarget->CustomizeInteractionEventData(TAG_Ability_Interaction_Activate, InPayload);

    // Grab the target actor off the payload we're going to use it as the 'avatar' for the interaction, and the
    // source InteractableTarget actor as the owner actor.
    AActor* TargetActor = const_cast<AActor*>(ToRawPtr(InPayload.Target));

    // The actor info needed for the interaction.

    InActorInfo.InitFromActor(InteractableTargetActor, TargetActor, InInteractionOption.TargetAbilitySystem);
}

