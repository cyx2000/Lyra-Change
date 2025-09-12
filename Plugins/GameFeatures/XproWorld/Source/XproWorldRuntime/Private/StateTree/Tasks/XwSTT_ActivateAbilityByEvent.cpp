#include "StateTree/Tasks/XwSTT_ActivateAbilityByEvent.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeAsyncExecutionContext.h"
#include "AI/XwAICharacter.h"
#include "AbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(XwSTT_ActivateAbilityByEvent)

FXwSTT_ActivateAbilityByEvent::FXwSTT_ActivateAbilityByEvent()
{
	bShouldCallTick = false;
    bShouldCopyBoundPropertiesOnTick = false;
	bShouldCopyBoundPropertiesOnExitState = false;
}

EStateTreeRunStatus FXwSTT_ActivateAbilityByEvent::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	int32 ActivateCount{};
	
	{
		FGameplayEventData NoneData{};

		ActivateCount = InstanceData.Actor->GetAbilitySystemComponent()->HandleGameplayEvent(InstanceData.AbilityTag, &NoneData);
	}

	if(ActivateCount > 0)
	{
		UWorld* World = Context.GetWorld();

		if (UGameplayMessageSubsystem::HasInstance(World))
		{
			UGameplayMessageSubsystem& Router = UGameplayMessageSubsystem::Get(World);
			InstanceData.ListenerHandle = Router.RegisterListenerInternal(InstanceData.AbilityTag,
					[WeakThis = Context.MakeWeakExecutionContext()](FGameplayTag Channel, const UScriptStruct* StructType, const void* Payload)
					{		
						auto AA = (const FXwST_StateTreeTaskData*)Payload;
						WeakThis.FinishTask(AA->bFinished ? EStateTreeFinishTaskType::Succeeded : EStateTreeFinishTaskType::Failed);
					},
					MessageStructType.Get(),
					MessageMatchType);
		}
	}
	else 
	{
		Context.FinishTask(*this, EStateTreeFinishTaskType::Failed);
	}

	return EStateTreeRunStatus::Running;
}

void FXwSTT_ActivateAbilityByEvent::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	InstanceData.ListenerHandle.Unregister();
}
