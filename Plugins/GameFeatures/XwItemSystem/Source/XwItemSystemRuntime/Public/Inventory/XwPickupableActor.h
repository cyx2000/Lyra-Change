// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Interaction/IInteractableTarget.h"
#include "Interaction/InteractionOption.h"
#include "Inventory/IXWPickupable.h"
#include "XwPickupableActor.generated.h"

UCLASS(MinimalAPI, Abstract)
class AXwPickupableActor : public AActor, public IInteractableTarget, public IXWPickupable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	XWITEMSYSTEMRUNTIME_API AXwPickupableActor();

	virtual void GatherInteractionOptions(const FInteractionQuery& InteractQuery, FInteractionOptionBuilder& InteractionBuilder) override;
	virtual FXWInventoryPickup GetPickupInventory() const override;

	XWITEMSYSTEMRUNTIME_API void AddPickupItemByDef(TSubclassOf<UXWorldInventoryItemDefinition> InDef, uint8 InCount = 1);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Gameplay)
	TObjectPtr<UStaticMesh> DisplayMesh;

protected:
	UPROPERTY(EditAnywhere, Category=Gameplay)
	FInteractionOption Option;

	UPROPERTY(EditAnywhere, Category=Gameplay)
	FXWInventoryPickup StaticInventory;

};
