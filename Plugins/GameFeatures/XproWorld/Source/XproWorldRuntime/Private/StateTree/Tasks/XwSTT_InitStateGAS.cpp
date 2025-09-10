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

	const AXwAICharacter* ControledPawn = InstanceData.AIController->GetPawn<AXwAICharacter>();

	ULyraAbilitySystemComponent* EnemyASC = ControledPawn->GetLyraAbilitySystemComponent();
	
	{
		if(auto EnemyDefaultArrRef = ControledPawn->XpAIBag.GetArrayRef(CurrentStateName); EnemyDefaultArrRef.IsValid())
		{
			auto& EnemyDefaultDataArr = EnemyDefaultArrRef.GetValue();

			const auto ArrNum = EnemyDefaultDataArr.Num();
			for(int32 Idx = 0; Idx < ArrNum; ++Idx)
			{
				const auto& StateActionGASName = EnemyDefaultDataArr.GetValueName(Idx).GetValue();

				const auto& DefaultGASSoftPath = StateDataItem->XpBag.GetValueSoftPath(StateActionGASName).GetValue();

				auto& WantHandle = InstanceData.CurrentStateAbilitySet.FindOrAdd(StateActionGASName);

				Cast<ULyraAbilitySet>(DefaultGASSoftPath.TryLoad())->GiveToAbilitySystem(EnemyASC, &WantHandle);
			}
		}
	}

	{
		auto& CurrentPawnName = ControledPawn->EnemyName;

		WantID.ItemName = CurrentPawnName;
		const FXpWorldInstanceBag* EnemyPawnData = DRsystem->GetCachedItem<FXpWorldInstanceBag>(WantID);

		if(EnemyPawnData)
		{
			auto NameArrRes = EnemyPawnData->XpBag.GetArrayRef(CurrentStateName).TryGetValue();

			const auto ArrNum = NameArrRes->Num();
			for(int32 Idx = 0; Idx < ArrNum; ++Idx)
			{
				const auto& StateActionGASName = NameArrRes->GetValueName(Idx).GetValue();

				const auto& GASSoftPath = StateDataItem->XpBag.GetValueSoftPath(StateActionGASName).GetValue();

				auto& WantHandle = InstanceData.CurrentStateAbilitySet.FindOrAdd(StateActionGASName);

				Cast<ULyraAbilitySet>(GASSoftPath.TryLoad())->GiveToAbilitySystem(EnemyASC, &WantHandle);
			}
		}
	}
	
	const AActor* Spawner = InstanceData.AIController->GetOwner();
	
	for (const auto& StateTag : Spawner->Tags)
	{
		if(auto TagGASSoftPath = StateDataItem->XpBag.GetValueSoftPath(StateTag); TagGASSoftPath.IsValid())
		{
			auto& WantHandle = InstanceData.CurrentStateAbilitySet.FindOrAdd(StateTag);

			const auto& GASSoftPath = TagGASSoftPath.GetValue();

			Cast<ULyraAbilitySet>(GASSoftPath.TryLoad())->GiveToAbilitySystem(EnemyASC, &WantHandle);
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


