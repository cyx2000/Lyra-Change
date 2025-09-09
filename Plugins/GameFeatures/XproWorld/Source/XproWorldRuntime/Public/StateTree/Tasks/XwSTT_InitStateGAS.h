#pragma once

#include "StateTree/XwSTTaskBase.h"
#include "AbilitySystem/LyraAbilitySet.h"
#include "XwSTT_InitStateGAS.generated.h"

class AAIController;

USTRUCT()
struct XPROWORLDRUNTIME_API FXwST_InitStateGASInstanceData
{
	GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AAIController> AIController = nullptr;

	UPROPERTY()
	TMap<FName, FLyraAbilitySet_GrantedHandles> CurrentStateAbilitySet;

};

USTRUCT(meta = (DisplayName = "XwSTT_InitStateGAS", Category = "Xp|AI"))
struct FXwSTT_InitStateGAS : public FXwSTTaskBase
{
	GENERATED_BODY()

    XPROWORLDRUNTIME_API FXwSTT_InitStateGAS();

    using FInstanceDataType = FXwST_InitStateGASInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FXwST_InitStateGASInstanceData::StaticStruct(); }

    XPROWORLDRUNTIME_API virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	XPROWORLDRUNTIME_API virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR
	virtual EDataValidationResult Compile(UE::StateTree::ICompileNodeContext& Context) override;

	virtual FColor GetIconColor() const override
	{
		return UE::StateTree::Colors::Grey;
	}
#endif // WITH_EDITOR

};