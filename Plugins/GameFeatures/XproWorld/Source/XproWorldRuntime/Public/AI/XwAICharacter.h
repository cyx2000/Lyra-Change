// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Character/LyraCharacter.h"
#include "XwAICharacter.generated.h"

class ULyraPawnData;
/**
 * 
 */
UCLASS(MinimalAPI, Abstract, Blueprintable)
class AXwAICharacter : public ALyraCharacter
{
	GENERATED_BODY()

public:

	XPROWORLDRUNTIME_API AXwAICharacter(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/* @Game-Change start since the ability system lives here and not on the playerState we want to set the ability set here */
	static const FName NAME_LyraAbilityReady;
	
	void SetPawnData(const ULyraPawnData* InPawnData);

	TFunction<void(AActor*)> BeforeDestory;

protected:
	virtual void OnAbilitySystemInitialized() override;

	virtual void OnDeathStarted(AActor* OwningActor) override;

	/* @Game-Change end since the ability system lives here and not on the playerState we want to set the ability set here */
private:

	// The ability system component sub-object used by player characters.
	UPROPERTY(VisibleAnywhere, Category = "XproWorld|Character")
	TObjectPtr<ULyraAbilitySystemComponent> AbilitySystemComponent;

	// Health attribute set used by this actor.
	UPROPERTY()
	TObjectPtr<const class ULyraHealthSet> HealthSet;
	// Combat attribute set used by this actor.
	UPROPERTY()
	TObjectPtr<const class ULyraCombatSet> CombatSet;
};
