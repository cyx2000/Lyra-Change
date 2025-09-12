#pragma once

#include "StateTree/XwSTTaskBase.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/GameplayMessageTypes2.h"
#include "XwSTT_ActivateAbilityByEvent.generated.h"

class AXwAICharacter;

USTRUCT(BlueprintType)
struct XPROWORLDRUNTIME_API FXwST_StateTreeTaskData
{
	GENERATED_BODY()

	FXwST_StateTreeTaskData() {}
	FXwST_StateTreeTaskData(bool InFinished)
	:bFinished(InFinished)
	{

	}

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFinished = false;

};

USTRUCT()
struct XPROWORLDRUNTIME_API FXwST_AbilityInstanceData
{
	GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AXwAICharacter> Actor;

	UPROPERTY(EditAnywhere, Category = Parameters)
	FGameplayTag AbilityTag;

	FGameplayMessageListenerHandle ListenerHandle;
};

USTRUCT(meta = (DisplayName = "XwSTT_ActivateAbilityByEvent", Category = "Xp|AI"))
struct FXwSTT_ActivateAbilityByEvent : public FXwSTTaskBase
{
	GENERATED_BODY()

    XPROWORLDRUNTIME_API FXwSTT_ActivateAbilityByEvent();

    using FInstanceDataType = FXwST_AbilityInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FXwST_AbilityInstanceData::StaticStruct(); }

    XPROWORLDRUNTIME_API virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	XPROWORLDRUNTIME_API virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
#if WITH_EDITOR
	virtual EDataValidationResult Compile(UE::StateTree::ICompileNodeContext& Context) override
    {
        return EDataValidationResult::Valid;
    }

	virtual FColor GetIconColor() const override
	{
		return UE::StateTree::Colors::Grey;
	}
#endif // WITH_EDITOR

private:

	TWeakObjectPtr<UScriptStruct> MessageStructType = FXwST_StateTreeTaskData::StaticStruct();
	EGameplayMessageMatch MessageMatchType = EGameplayMessageMatch::ExactMatch;

};