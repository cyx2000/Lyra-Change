// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "XpWorldInteractionStatics.generated.h"

class IInteractableTarget;

struct FOverlapResult;
/**
 * 
 */
UCLASS()
class UXpWorldInteractionStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UXpWorldInteractionStatics();

	static void GetInteractableTargetsFromOverlapResults(const TArray<FOverlapResult>& InOverlapResults, TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets);

	static void GetInteractableTargetsFromActor(AActor* InActor, TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets);

	static void GetInteractableTargetsFromComponent(UPrimitiveComponent* InComponent, TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets);

	static void GetInteractableTargetsFromActorOrComponent(AActor* InActor, UPrimitiveComponent* InComponent, TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets);
	
};
