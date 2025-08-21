// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystem/Attributes/LyraAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "XpWorldCombatSet.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI)
class  UXpWorldCombatSet : public ULyraAttributeSet
{
	GENERATED_BODY()

public:
	XPROWORLDRUNTIME_API UXpWorldCombatSet();

	ATTRIBUTE_ACCESSORS(UXpWorldCombatSet, Attack);
	ATTRIBUTE_ACCESSORS(UXpWorldCombatSet, Defense);

protected:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Attack(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Defense(const FGameplayAttributeData& OldValue);

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	// virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(float& NewValue) const;

private:

	// The base amount of damage to apply in the damage execution.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attack, Category = "XpWorld|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attack;

	// The base amount of healing to apply in the heal execution.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Defense, Category = "XpWorld|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Defense;


	
};
