// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/XpWorldInteractionStatics.h"
#include "Engine/OverlapResult.h"
#include "Interaction/IInteractableTarget.h"
#include "Inventory/IXWPickupable.h"
#include "Logging/LogVerbosity.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(XpWorldInteractionStatics)


UXpWorldInteractionStatics::UXpWorldInteractionStatics()
    : Super(FObjectInitializer::Get())
{

}

void UXpWorldInteractionStatics::GetInteractableTargetsFromOverlapResults(const TArray<FOverlapResult>& InOverlapResults, TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets)
{
    for (const FOverlapResult& Overlap : InOverlapResults)
	{
        AActor* OverlapActor = Overlap.GetActor();
        
        TScriptInterface<IXWPickupable> PickupActor(OverlapActor);

        if(PickupActor)
        {
            OutInteractableTargets.Reset();
            GetInteractableTargetsFromActor(OverlapActor, OutInteractableTargets);
            return;
        }

        UPrimitiveComponent* OverlapComp = Overlap.GetComponent();
        TScriptInterface<IXWPickupable> PickupComp(OverlapComp);
        
        if(PickupComp)
        {
            OutInteractableTargets.Reset();
            GetInteractableTargetsFromComponent(OverlapComp, OutInteractableTargets);
            return;
        }

        GetInteractableTargetsFromActorOrComponent(OverlapActor, OverlapComp, OutInteractableTargets);

	}
}

void UXpWorldInteractionStatics::GetInteractableTargetsFromActor(AActor* InActor, TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets)
{
    TScriptInterface<IInteractableTarget> InteractableActor(InActor);
	if (InteractableActor)
	{
		OutInteractableTargets.AddUnique(InteractableActor);
	}
}

void UXpWorldInteractionStatics::GetInteractableTargetsFromComponent(UPrimitiveComponent* InComponent, TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets)
{
    TScriptInterface<IInteractableTarget> InteractableComponent(InComponent);
	if (InteractableComponent)
	{
		OutInteractableTargets.AddUnique(InteractableComponent);
	}

    unimplemented();
}

void UXpWorldInteractionStatics::GetInteractableTargetsFromActorOrComponent(AActor* InActor, UPrimitiveComponent* InComponent, TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets)
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
}


