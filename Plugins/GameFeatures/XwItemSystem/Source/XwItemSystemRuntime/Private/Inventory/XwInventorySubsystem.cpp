// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/XwInventorySubsystem.h"
#include "Inventory/XWorldInventoryManagerComponent.h"


const UXWorldInventoryItemFragment* UXwInventorySubsystem::FindItemDefinitionFragment(TSubclassOf<UXWorldInventoryItemDefinition> ItemDef, TSubclassOf<UXWorldInventoryItemFragment> FragmentClass)
{
    if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<UXWorldInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}
