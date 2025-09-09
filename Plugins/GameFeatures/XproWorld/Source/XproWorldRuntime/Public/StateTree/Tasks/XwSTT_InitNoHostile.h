#pragma once

#include "StateTree/XwSTTaskBase.h"
#include "StateTreePropertyRef.h"
#include "XwSTT_InitNoHostile.generated.h"

class AAIController;

USTRUCT()
struct XPROWORLDRUNTIME_API FXwST_InitNoHostileInstanceData
{
	GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AAIController> AIController = nullptr;

	UPROPERTY(EditAnywhere, Category = Parameter)
	TStateTreePropertyRef<FVector> SpawnerLocation;

    UPROPERTY(EditAnywhere, Category = Parameters)
	TStateTreePropertyRef<int32> CurrentSplineInputKey;

	UPROPERTY(EditAnywhere, Category = Parameters)
	TStateTreePropertyRef<int32> SplineInputKeySize;

    UPROPERTY(EditAnywhere, Category = Parameter)
	FGameplayTag SplineDescendantTag;
};

USTRUCT(meta = (DisplayName = "XwSTT_InitNoHostile", Category = "Xp|AI"))
struct FXwSTT_InitNoHostile : public FXwSTTaskBase
{
	GENERATED_BODY()

    XPROWORLDRUNTIME_API FXwSTT_InitNoHostile();

    using FInstanceDataType = FXwST_InitNoHostileInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FXwST_InitNoHostileInstanceData::StaticStruct(); }

    XPROWORLDRUNTIME_API virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR
	virtual EDataValidationResult Compile(UE::StateTree::ICompileNodeContext& Context) override;

	virtual FColor GetIconColor() const override
	{
		return UE::StateTree::Colors::Grey;
	}
#endif // WITH_EDITOR

};