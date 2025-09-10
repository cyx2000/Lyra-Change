// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attribute/XpWorldMoveSet.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(XpWorldMoveSet)

UXpWorldMoveSet::UXpWorldMoveSet()
    :AccelerationSpeed(0.f),
    MoveSpeed(0.f)
{

}

void UXpWorldMoveSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, AccelerationSpeed, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MoveSpeed, COND_OwnerOnly, REPNOTIFY_Always);
}

void UXpWorldMoveSet::OnRep_AccelerationSpeed(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, AccelerationSpeed, OldValue);
}

void UXpWorldMoveSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MoveSpeed, OldValue);
}

void UXpWorldMoveSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
    if(Attribute == GetAccelerationSpeedAttribute())
    {
        NewValue = FMath::Clamp(NewValue, -100.0f, 200.f);
    }
    else if(Attribute == GetMoveSpeedAttribute())
    {
        NewValue = FMath::Clamp(NewValue, -100.0f, 100.f);
    }
}

void UXpWorldMoveSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
    Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UXpWorldMoveSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}
