// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/LyraAttributeSet.h"
#include "XpWorldDaElementalSet.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI)
class UXpWorldDaElementalSet : public ULyraAttributeSet
{
	GENERATED_BODY()

public:

	XPROWORLDRUNTIME_API UXpWorldDaElementalSet();

	ATTRIBUTE_ACCESSORS(UXpWorldDaElementalSet, DamageElemental);

protected:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_DamageElemental(const FGameplayAttributeData& OldValue);
	
private:

	//0-100%
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DamageElemental, Category = "XpWorld|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData DamageElemental;
};
