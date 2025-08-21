// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/XwPickupableActor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(XwPickupableActor)

// Sets default values
AXwPickupableActor::AXwPickupableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

}

void AXwPickupableActor::GatherInteractionOptions(const FInteractionQuery& InteractQuery, FInteractionOptionBuilder& InteractionBuilder)
{
	InteractionBuilder.AddInteractionOption(Option);
}

FXWInventoryPickup AXwPickupableActor::GetPickupInventory() const
{
	return StaticInventory;
}

void AXwPickupableActor::AddPickupItemByDef(TSubclassOf<UXWorldInventoryItemDefinition> InDef, uint8 InCount)
{
	FXWPickupTemplate NewPickDef;
	NewPickDef.ItemDef = InDef;
	NewPickDef.StackCount = static_cast<int32>(InCount);
	StaticInventory.Templates.Emplace(NewPickDef);
}
