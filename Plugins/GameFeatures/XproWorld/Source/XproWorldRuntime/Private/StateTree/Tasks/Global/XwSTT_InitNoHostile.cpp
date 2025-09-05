#include "StateTree/Tasks/Global/XwSTT_InitNoHostile.h"
#include "AI/XwAISpawner.h"
#include "AIController.h"
#include "AI/XwAICharacter.h"
#include "AbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "GameplayTagContainer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(XwSTT_InitNoHostile)

FXwSTT_InitNoHostile::FXwSTT_InitNoHostile()
{
	bShouldCallTick = false;
    bShouldCopyBoundPropertiesOnTick = false;
	bShouldCopyBoundPropertiesOnExitState = false;
}

EStateTreeRunStatus FXwSTT_InitNoHostile::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

    check(InstanceData.AIController);

	AXwAISpawner* Spawner = Cast<AXwAISpawner>(InstanceData.AIController->GetOwner());

	*InstanceData.SpawnerLocation.GetMutablePtr(Context) = Spawner->GetActorLocation();

	const AXwAICharacter* CurrentPawn = InstanceData.AIController->GetPawn<AXwAICharacter>();

    UAbilitySystemComponent* PawnASC = CurrentPawn->GetAbilitySystemComponent();

	const FGameplayTagContainer& CurrentTags = PawnASC->GetOwnedGameplayTags();
	if(CurrentTags.HasTag(InstanceData.SplineTag))
	{
		USplineComponent* DefaultSpline = Spawner->FindComponentByClass<USplineComponent>();
		check(DefaultSpline);
		*InstanceData.CurrentSplineInputKey.GetMutablePtr(Context) = static_cast<int32>(DefaultSpline->FindInputKeyClosestToWorldLocation(
			CurrentPawn->GetActorLocation()));

		*InstanceData.SplineInputKeySize.GetMutablePtr(Context) = CurrentTags.HasTag(InstanceData.SplineDescendantTag) ? -1 : 1;
		
	}

	return EStateTreeRunStatus::Running;
}

#if WITH_EDITOR

EDataValidationResult FXwSTT_InitNoHostile::Compile(UE::StateTree::ICompileNodeContext& Context)
{
	const FInstanceDataType& InstanceData = Context.GetInstanceDataView().Get<FInstanceDataType>();

	return EDataValidationResult::Valid;
}

#endif // WITH_EDITOR
