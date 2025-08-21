// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ModularAIController.h"
#include "Teams/LyraTeamAgentInterface.h"
#include "XwAIController.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI, Abstract, Blueprintable)
class AXwAIController : public AModularAIController, public ILyraTeamAgentInterface
{
	GENERATED_BODY()

public:

	XPROWORLDRUNTIME_API AXwAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	//~ILyraTeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual FOnLyraTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	//~End of ILyraTeamAgentInterface interface

	//Update Team Attitude for the AI
	UFUNCTION(BlueprintCallable, Category = "Lyra AI Player Controller")
	void UpdateTeamAttitude(UAIPerceptionComponent* AIPerception);

private:
	// Keep track of TeamID if there's no PlayerState. No OnRep because AIController only exists on Server 
	UPROPERTY()
	FOnLyraTeamIndexChangedDelegate OnTeamChangedDelegate;
	
	UPROPERTY()
	FGenericTeamId MyTeamID;
};
