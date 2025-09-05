// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "StateTree/XwSTTaskBase.h"
#include "StateTreePropertyRef.h"
#include "XwSTT_GetMovePointOnSpawner.generated.h"

class AAIController;

USTRUCT()
struct XPROWORLDRUNTIME_API FXwST_GetRandomPointInstanceData
{
	GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AAIController> AIController = nullptr;

	UPROPERTY(EditAnywhere, Category = Parameter)
	FVector SpawnLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, Category = Output)
	FVector OutRandomLocation = FVector::ZeroVector;

};

USTRUCT(meta = (DisplayName = "XwSTT_GetRandomPointOnSpawner", Category = "Xp|AI"))
struct FXwSTT_GetRandomPointOnSpawner : public FXwSTTaskBase
{
	GENERATED_BODY()

    XPROWORLDRUNTIME_API FXwSTT_GetRandomPointOnSpawner();

    using FInstanceDataType = FXwST_GetRandomPointInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FXwST_GetRandomPointInstanceData::StaticStruct(); }

    XPROWORLDRUNTIME_API virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR
	virtual EDataValidationResult Compile(UE::StateTree::ICompileNodeContext& Context) override;

	virtual FColor GetIconColor() const override
	{
		return UE::StateTree::Colors::Grey;
	}
#endif // WITH_EDITOR

};

USTRUCT()
struct XPROWORLDRUNTIME_API FXwST_GetSplinePointInstanceData
{
	GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AAIController> AIController = nullptr;

	UPROPERTY(EditAnywhere, Category = Parameter)
	FName WantSplineName = NAME_None;

	UPROPERTY(EditAnywhere, Category = Input)
	TStateTreePropertyRef<int32> CurrentSplineInputKey;

	UPROPERTY(EditAnywhere, Category = Input)
	TStateTreePropertyRef<int32> SplineInputKeySize;

	UPROPERTY(VisibleAnywhere, Category = Output)
	FVector OutSplinePointLocation = FVector::ZeroVector;

};

USTRUCT(meta = (DisplayName = "XwSTT_GetSplinePointOnSpawner", Category = "Xp|AI"))
struct FXwSTT_GetSplinePointOnSpawner : public FXwSTTaskBase
{
	GENERATED_BODY()

    XPROWORLDRUNTIME_API FXwSTT_GetSplinePointOnSpawner();

    using FInstanceDataType = FXwST_GetSplinePointInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FXwST_GetSplinePointInstanceData::StaticStruct(); }

    XPROWORLDRUNTIME_API virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR
	virtual EDataValidationResult Compile(UE::StateTree::ICompileNodeContext& Context) override;

	virtual FColor GetIconColor() const override
	{
		return UE::StateTree::Colors::Grey;
	}
#endif // WITH_EDITOR

};