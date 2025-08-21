// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/LyraAttributeSet.h"
#include "XpWorldDaResistanceSet.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI)
class UXpWorldDaResistanceSet : public ULyraAttributeSet
{
	GENERATED_BODY()

public:

	XPROWORLDRUNTIME_API UXpWorldDaResistanceSet();

	ATTRIBUTE_ACCESSORS(UXpWorldDaResistanceSet, DamageResistance);

protected:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_DamageResistance(const FGameplayAttributeData& OldValue);
	
private:

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DamageResistance, Category = "XpWorld|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData DamageResistance;
};
