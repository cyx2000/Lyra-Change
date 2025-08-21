// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MMC/EquipmentAttrMMC.h"
#include "Equipment/XWorldEquipmentDefinition.h"
#include "Misc/AssertionMacros.h"

UEquipmentAttrMMC::UEquipmentAttrMMC()
{

}


float UEquipmentAttrMMC::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{

    const UXWorldEquipmentDefinition* CurrenDef = Cast<UXWorldEquipmentDefinition>(Spec.GetEffectContext().GetSourceObject());

    checkf(CurrenDef, TEXT("Only Equipment GE can use this MMC"));

    if(const float* WantSetValue = CurrenDef->Attributes.Find(WantAttribute))
    {
        return *WantSetValue;
    }

    return 0.f;
}
