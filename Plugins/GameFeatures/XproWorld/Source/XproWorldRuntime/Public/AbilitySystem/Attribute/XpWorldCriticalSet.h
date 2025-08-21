// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/LyraAttributeSet.h"
#include "XpWorldCriticalSet.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI)
class UXpWorldCriticalSet : public ULyraAttributeSet
{
	GENERATED_BODY()

public:
	XPROWORLDRUNTIME_API UXpWorldCriticalSet();

	ATTRIBUTE_ACCESSORS(UXpWorldCriticalSet, CriticalPercent);
	ATTRIBUTE_ACCESSORS(UXpWorldCriticalSet, CriticalDamage);
	
protected:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_CriticalPercent(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_CriticalDamage(const FGameplayAttributeData& OldValue);

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	// virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:

	//0-100%
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalPercent, Category = "XpWorld|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CriticalPercent;

	// 150%
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalDamage, Category = "XpWorld|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CriticalDamage;

};
