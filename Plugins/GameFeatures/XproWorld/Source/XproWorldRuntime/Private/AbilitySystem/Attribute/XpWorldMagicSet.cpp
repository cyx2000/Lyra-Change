// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attribute/XpWorldMagicSet.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(XpWorldMagicSet)

UXpWorldMagicSet::UXpWorldMagicSet()
{

}

void UXpWorldMagicSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, UnleashSpeed, COND_OwnerOnly, REPNOTIFY_Always);
}

void UXpWorldMagicSet::OnRep_UnleashSpeed(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, UnleashSpeed, OldValue);
}

void UXpWorldMagicSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
    if (Attribute == GetUnleashSpeedAttribute())
	{
		// Do not allow health to go negative or above max health.
		NewValue = FMath::Clamp(NewValue, -100.0f, 50.f);
	}
}

void UXpWorldMagicSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
    Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UXpWorldMagicSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}
