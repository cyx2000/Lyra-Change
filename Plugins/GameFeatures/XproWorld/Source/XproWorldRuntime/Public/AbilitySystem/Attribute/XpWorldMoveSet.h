// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/LyraAttributeSet.h"
#include "XpWorldMoveSet.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI)
class UXpWorldMoveSet : public ULyraAttributeSet
{
	GENERATED_BODY()

public:
	XPROWORLDRUNTIME_API UXpWorldMoveSet();

	ATTRIBUTE_ACCESSORS(UXpWorldMoveSet, AccelerationSpeed);
	ATTRIBUTE_ACCESSORS(UXpWorldMoveSet, MoveSpeed);

protected:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_AccelerationSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	// virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:

	//-100-200%
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AccelerationSpeed, Category = "XpWorld|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AccelerationSpeed;

	//-100-100%
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MoveSpeed, Category = "XpWorld|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MoveSpeed;
};
