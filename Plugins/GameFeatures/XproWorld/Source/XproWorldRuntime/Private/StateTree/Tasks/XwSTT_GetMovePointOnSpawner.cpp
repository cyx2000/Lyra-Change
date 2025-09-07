
#include "StateTree/Tasks/XwSTT_GetMovePointOnSpawner.h"

#include "AI/XwAICharacter.h"
#include "HAL/Platform.h"
#include "Misc/AssertionMacros.h"
#include "StateTreeExecutionContext.h"
#include "AI/XwAISpawner.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "Components/SplineComponent.h"
#include "AbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(XwSTT_GetMovePointOnSpawner)

FXwSTT_GetRandomPointOnSpawner::FXwSTT_GetRandomPointOnSpawner()
{
    bShouldCallTick = false;
    bShouldCopyBoundPropertiesOnTick = false;
	bShouldCopyBoundPropertiesOnExitState = false;
}

EStateTreeRunStatus FXwSTT_GetRandomPointOnSpawner::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
    FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

    check(InstanceData.AIController);

    AXwAISpawner* Spawner = Cast<AXwAISpawner>(InstanceData.AIController->GetOwner());

    check(Spawner->LocationBound > 50.f);

    FNavLocation RandomPoint(InstanceData.SpawnLocation);
    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(InstanceData.AIController->GetWorld());
    if (NavSys)
    {
        NavSys->GetRandomReachablePointInRadius(InstanceData.SpawnLocation, 
        Spawner->LocationBound, 
        RandomPoint, 
        NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate));
        InstanceData.OutRandomLocation = RandomPoint.Location;
    }
    Context.FinishTask(*this, EStateTreeFinishTaskType::Succeeded);
    return EStateTreeRunStatus::Running;
}

#if WITH_EDITOR

EDataValidationResult FXwSTT_GetRandomPointOnSpawner::Compile(UE::StateTree::ICompileNodeContext& Context)
{
	const FInstanceDataType& InstanceData = Context.GetInstanceDataView().Get<FInstanceDataType>();

	return EDataValidationResult::Valid;
}

#endif // WITH_EDITOR


//FXwSTT_GetSplinePointOnSpawner

FXwSTT_GetSplinePointOnSpawner::FXwSTT_GetSplinePointOnSpawner()
{
    bShouldCallTick = false;
    bShouldCopyBoundPropertiesOnTick = false;
	bShouldCopyBoundPropertiesOnExitState = false;
}

EStateTreeRunStatus FXwSTT_GetSplinePointOnSpawner::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
    FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

    check(InstanceData.AIController);

    USplineComponent* TargetSpline{ nullptr };

    {
        AXwAISpawner* Spawner = Cast<AXwAISpawner>(InstanceData.AIController->GetOwner());

        const FName& WantSplineName = InstanceData.WantSplineName;

        if(WantSplineName.IsNone())
        {
            TargetSpline = Spawner->FindComponentByClass<USplineComponent>();
        }
        else
        {
            TargetSpline = Spawner->FindComponentByTag<USplineComponent>(WantSplineName);
        }
    }

    check(TargetSpline);
    check(TargetSpline->GetNumberOfSplinePoints() > 2);

    int32& CurrentInputKey = *InstanceData.CurrentSplineInputKey.GetMutablePtr(Context);

    int32& CurrentInputKeyAddSize = *InstanceData.SplineInputKeySize.GetMutablePtr(Context);

    {
        const int32 MaxInputKey = TargetSpline->GetNumberOfSplinePoints();
        
        const int32 FinalIndex = (MaxInputKey - 1);

        const bool bToFinal = CurrentInputKey >= FinalIndex;

        const bool bNeedReset = bToFinal || CurrentInputKey <= 0.f;

        if(bNeedReset && TargetSpline->ComponentHasTag(TEXT("CycleLoop"))) 
        {
            CurrentInputKey = CurrentInputKeyAddSize > 0 ? ( bToFinal ? 0 : 1) : (bToFinal ? (FinalIndex - 1) : FinalIndex);
        }
        else if(bNeedReset)
        {
            CurrentInputKeyAddSize = -CurrentInputKeyAddSize;
            CurrentInputKey += CurrentInputKeyAddSize;
        }
        else 
        {
            CurrentInputKey += CurrentInputKeyAddSize;
        }
    }

    InstanceData.OutSplinePointLocation = TargetSpline->GetLocationAtSplineInputKey(static_cast<float>(CurrentInputKey), ESplineCoordinateSpace::World);

    Context.FinishTask(*this, EStateTreeFinishTaskType::Succeeded);
    
    return EStateTreeRunStatus::Running;
}

#if WITH_EDITOR

EDataValidationResult FXwSTT_GetSplinePointOnSpawner::Compile(UE::StateTree::ICompileNodeContext& Context)
{
	const FInstanceDataType& InstanceData = Context.GetInstanceDataView().Get<FInstanceDataType>();

	return EDataValidationResult::Valid;
}

#endif // WITH_EDITOR