// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attribute/XpWorldCombatSet.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(XpWorldCombatSet)

UXpWorldCombatSet::UXpWorldCombatSet()
{

}

void UXpWorldCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Attack, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Defense, COND_OwnerOnly, REPNOTIFY_Always);
}

void UXpWorldCombatSet::OnRep_Attack(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Attack, OldValue);
}

void UXpWorldCombatSet::OnRep_Defense(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Defense, OldValue);
}



void UXpWorldCombatSet::ClampAttribute(float& NewValue) const
{
    NewValue = FMath::Max(NewValue, 0.0f);
}

void UXpWorldCombatSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
    Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(NewValue);
}

void UXpWorldCombatSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(NewValue);
}

