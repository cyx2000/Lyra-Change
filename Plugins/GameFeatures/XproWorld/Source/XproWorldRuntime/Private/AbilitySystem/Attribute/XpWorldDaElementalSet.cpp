// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attribute/XpWorldDaElementalSet.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(XpWorldDaElementalSet)

UXpWorldDaElementalSet::UXpWorldDaElementalSet()
{

}

void UXpWorldDaElementalSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, DamageElemental, COND_OwnerOnly, REPNOTIFY_Always);
}


void UXpWorldDaElementalSet::OnRep_DamageElemental(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, DamageElemental, OldValue);
}
