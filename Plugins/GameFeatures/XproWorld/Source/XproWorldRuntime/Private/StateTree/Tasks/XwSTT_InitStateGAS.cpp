#include "StateTree/Tasks/XwSTT_InitStateGAS.h"
#include "AIController.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "DataRegistrySubsystem.h"
#include "Misc/AssertionMacros.h"
#include "StateTreeExecutionContext.h"
#include "AI/XwAISpawner.h"
#include "Game/XpWorldItem.h"
#include "AI/XwAICharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(XwSTT_InitStateGAS)

FXwSTT_InitStateGAS::FXwSTT_InitStateGAS()
{
	bShouldCallTick = false;
    bShouldCopyBoundPropertiesOnTick = false;
	bShouldCopyBoundPropertiesOnExitState = false;
}

EStateTreeRunStatus FXwSTT_InitStateGAS::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

    check(InstanceData.AIController);

	const FName& CurrentStateName = Context.GetStateFromHandle(Context.GetCurrentlyProcessedState())->Name;

	UDataRegistrySubsystem* DRsystem = UDataRegistrySubsystem::Get();

	check(DRsystem);

	FDataRegistryId WantID(AXwAISpawner::DRAIDataName, CurrentStateName);

	const FXpWorldInstanceBag* StateDataItem = DRsystem->GetCachedItem<FXpWorldInstanceBag>(WantID);
	check(StateDataItem);
	const AActor* Spawner = InstanceData.AIController->GetOwner();

	const AXwAICharacter* ControledPawn = InstanceData.AIController->GetPawn<AXwAICharacter>();

	for (const auto& StateTag : ControledPawn->Tags)
	{
		if(auto TagGASPath = StateDataItem->XpBag.GetValueSoftPath(StateTag); TagGASPath.IsValid())
		{
			auto& WantHandle = InstanceData.CurrentStateAbilitySet.FindOrAdd(StateTag);
			
			Cast<ULyraAbilitySet>(TagGASPath.GetValue().TryLoad())->GiveToAbilitySystem(ControledPawn->GetLyraAbilitySystemComponent(), &WantHandle);
		}
	}

	for (const auto& StateTag : Spawner->Tags)
	{
		if(auto TagGASPath = StateDataItem->XpBag.GetValueSoftPath(StateTag); TagGASPath.IsValid())
		{
			auto& WantHandle = InstanceData.CurrentStateAbilitySet.FindOrAdd(StateTag);
			
			Cast<ULyraAbilitySet>(TagGASPath.GetValue().TryLoad())->GiveToAbilitySystem(ControledPawn->GetLyraAbilitySystemComponent(), &WantHandle);
		}
	}

	Context.FinishTask(*this, EStateTreeFinishTaskType::Succeeded);

	return EStateTreeRunStatus::Running;
}

void FXwSTT_InitStateGAS::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	check(InstanceData.AIController);

	const AXwAICharacter* ControledPawn = InstanceData.AIController->GetPawn<AXwAICharacter>();

	ULyraAbilitySystemComponent* ASC = ControledPawn->GetLyraAbilitySystemComponent();

	check(ASC);

	for(auto& AbilitySetHandle : InstanceData.CurrentStateAbilitySet)
	{
		AbilitySetHandle.Value.TakeFromAbilitySystem(ASC);
	}
}

#if WITH_EDITOR

EDataValidationResult FXwSTT_InitStateGAS::Compile(UE::StateTree::ICompileNodeContext& Context)
{
	const FInstanceDataType& InstanceData = Context.GetInstanceDataView().Get<FInstanceDataType>();

	return EDataValidationResult::Valid;
}

#endif // WITH_EDITOR


