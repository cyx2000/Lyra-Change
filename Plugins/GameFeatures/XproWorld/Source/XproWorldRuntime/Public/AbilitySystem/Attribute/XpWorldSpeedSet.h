// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/LyraAttributeSet.h"
#include "XpWorldSpeedSet.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI)
class UXpWorldSpeedSet : public ULyraAttributeSet
{
	GENERATED_BODY()

public:
	XPROWORLDRUNTIME_API UXpWorldSpeedSet();

	ATTRIBUTE_ACCESSORS(UXpWorldSpeedSet, AttackSpeed);
	ATTRIBUTE_ACCESSORS(UXpWorldSpeedSet, MoveSpeed);

protected:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_AttackSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	// virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:

	//-100-50%
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackSpeed, Category = "XpWorld|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackSpeed;

	//-100-100%
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MoveSpeed, Category = "XpWorld|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MoveSpeed;
};
