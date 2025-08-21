// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/LyraAttributeSet.h"
#include "XpWorldMagicSet.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI)
class UXpWorldMagicSet : public ULyraAttributeSet
{
	GENERATED_BODY()

public:

	XPROWORLDRUNTIME_API UXpWorldMagicSet();

	ATTRIBUTE_ACCESSORS(UXpWorldMagicSet, UnleashSpeed);

protected:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_UnleashSpeed(const FGameplayAttributeData& OldValue);

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	// virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:

	//-100-100%
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_UnleashSpeed, Category = "XpWorld|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData UnleashSpeed;
	
};
