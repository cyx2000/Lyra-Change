// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attribute/XpWorldCriticalSet.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(XpWorldCriticalSet)

UXpWorldCriticalSet::UXpWorldCriticalSet()
{

}

void UXpWorldCriticalSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, CriticalPercent, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, CriticalDamage, COND_OwnerOnly, REPNOTIFY_Always);
}

void UXpWorldCriticalSet::OnRep_CriticalPercent(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, CriticalPercent, OldValue);
}

void UXpWorldCriticalSet::OnRep_CriticalDamage(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, CriticalDamage, OldValue);
}

void UXpWorldCriticalSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
    if(Attribute == GetCriticalPercentAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, 100.f);
    }
    else if(Attribute == GetCriticalDamageAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 100.0f, 150.f);
    }
}

void UXpWorldCriticalSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
    Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UXpWorldCriticalSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}
