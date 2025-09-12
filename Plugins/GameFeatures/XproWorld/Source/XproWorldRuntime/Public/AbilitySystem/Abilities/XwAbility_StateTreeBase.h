// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "XwAbility_StateTreeBase.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI, Abstract)
class UXwAbility_StateTreeBase : public ULyraGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void K2_EndAbility() override
	{
		checkNoEntry();
	}
	virtual void K2_EndAbilityLocally() override
	{
		checkNoEntry();
	}

	UFUNCTION(BlueprintCallable, Category = Ability)
	void EndStateTreeTaskAbilityLocally(bool bFinishedTask);

private:

	void BroadcastTaskAbilityFinished(const FGameplayTag& InEventTag, const bool bFinished);
};
