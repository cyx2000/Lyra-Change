// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "XwInventorySubsystem.generated.h"

class UXWorldInventoryManagerComponent;
class UXWorldInventoryItemDefinition;
class UXWorldInventoryItemFragment;
/**
 * 
 */
UCLASS()
class XWITEMSYSTEMRUNTIME_API UXwInventorySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()


protected:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

public:
	UFUNCTION(BlueprintCallable, meta=(DeterminesOutputType=FragmentClass))
	const UXWorldInventoryItemFragment* FindItemDefinitionFragment(TSubclassOf<UXWorldInventoryItemDefinition> ItemDef, TSubclassOf<UXWorldInventoryItemFragment> FragmentClass);
	
};
