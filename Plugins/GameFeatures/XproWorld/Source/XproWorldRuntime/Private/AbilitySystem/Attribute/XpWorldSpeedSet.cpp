// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attribute/XpWorldSpeedSet.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(XpWorldSpeedSet)

UXpWorldSpeedSet::UXpWorldSpeedSet()
{

}

void UXpWorldSpeedSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, AttackSpeed, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MoveSpeed, COND_OwnerOnly, REPNOTIFY_Always);
}

void UXpWorldSpeedSet::OnRep_AttackSpeed(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, AttackSpeed, OldValue);
}

void UXpWorldSpeedSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MoveSpeed, OldValue);
}

void UXpWorldSpeedSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
    if(Attribute == GetAttackSpeedAttribute())
    {
        NewValue = FMath::Clamp(NewValue, -100.0f, 50.f);
    }
    else if(Attribute == GetMoveSpeedAttribute())
    {
        NewValue = FMath::Clamp(NewValue, -100.0f, 100.f);
    }
}

void UXpWorldSpeedSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
    Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UXpWorldSpeedSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}
