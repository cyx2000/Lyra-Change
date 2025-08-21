// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayModMagnitudeCalculation.h"
#include "EquipmentAttrMMC.generated.h"

/**
 * 
 */
UCLASS(Abstract, MinimalAPI)
class UEquipmentAttrMMC : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:

	UEquipmentAttrMMC();

	XWITEMSYSTEMRUNTIME_API float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Equipment")
	FGameplayAttribute WantAttribute;
};
