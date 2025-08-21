// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Tasks/AT_WFITargets_Overlap.h"
#include "Physics/LyraCollisionChannels.h"
#include "Interaction/XpWorldInteractionStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AT_WFITargets_Overlap)

UAT_WFITargets_Overlap::UAT_WFITargets_Overlap(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UAT_WFITargets_Overlap::Activate()
{
    PerformOverlap();
    EndTask();
}
UAT_WFITargets_Overlap* UAT_WFITargets_Overlap::WaitForInteractableTargets_Overlap(UGameplayAbility* OwningAbility, 
    FInteractionQuery InteractionQuery, 
    FGameplayAbilityTargetingLocationInfo StartLocation, 
    float InteractionScanRange)
{
    UAT_WFITargets_Overlap* MyObj = NewAbilityTask<UAT_WFITargets_Overlap>(OwningAbility);
	MyObj->InteractionScanRange = InteractionScanRange;
	MyObj->StartLocation = StartLocation;
	MyObj->InteractionQuery = InteractionQuery;

	return MyObj;
}

void UAT_WFITargets_Overlap::PerformOverlap()
{
    AActor* AvatarActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	if (!AvatarActor)
	{
		return;
	}
    UWorld* World = GetWorld();
    FCollisionQueryParams Params(SCENE_QUERY_STAT(UAT_WFITargets_Overlap), false);
    TArray<FOverlapResult> OverlapResults;
    World->OverlapMultiByChannel(OUT OverlapResults, AvatarActor->GetActorLocation(), FQuat::Identity, Lyra_TraceChannel_Interaction, FCollisionShape::MakeSphere(InteractionScanRange), Params);

    if (OverlapResults.Num() > 0)
    {
        TArray<TScriptInterface<IInteractableTarget>> InteractableTargets;
        UXpWorldInteractionStatics::GetInteractableTargetsFromOverlapResults(OverlapResults, InteractableTargets);

        UpdateInteractableOptions(InteractionQuery, InteractableTargets);
    }
}
