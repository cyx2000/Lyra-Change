// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attribute/XpWorldDaResistanceSet.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(XpWorldDaResistanceSet)

UXpWorldDaResistanceSet::UXpWorldDaResistanceSet()
{

}

void UXpWorldDaResistanceSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, DamageResistance, COND_OwnerOnly, REPNOTIFY_Always);
}

void UXpWorldDaResistanceSet::OnRep_DamageResistance(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, DamageResistance, OldValue);
}
