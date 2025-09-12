// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/XwAIController.h"
#include "AI/XwAICharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Components/StateTreeAIComponent.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AISense_Sight.h"
#include "XpWorldTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(XwAIController)

AXwAIController::AXwAIController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
    bWantsPlayerState = false;
	bStopAILogicOnUnposses = false;

    MyTeamID = FGenericTeamId::NoTeam;

	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIperception"));

	StateTreeAIComp = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StatetreeAI"));

}

void AXwAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	PerceptionComponent->OnTargetPerceptionInfoUpdated.AddDynamic(this, &ThisClass::HandleTargetPerceptionInfoUpdated);

}

void AXwAIController::OnUnPossess()
{
	PerceptionComponent->OnTargetPerceptionInfoUpdated.RemoveAll(this);

	Super::OnUnPossess();
}

void AXwAIController::UpdateTeamAttitude(UAIPerceptionComponent* AIPerception)
{
    if (AIPerception)
	{
		AIPerception->RequestStimuliListenerUpdate();
	}
}

void AXwAIController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
    check(HasAuthority());

	{
		const FGenericTeamId OldTeamID = MyTeamID;

		MyTeamID = NewTeamID;
		ConditionalBroadcastTeamChanged(this, OldTeamID, NewTeamID);
	}
}

FGenericTeamId AXwAIController::GetGenericTeamId() const
{
    return MyTeamID;
}

FOnLyraTeamIndexChangedDelegate* AXwAIController::GetOnTeamIndexChangedDelegate()
{
    return &OnTeamChangedDelegate;
}

ETeamAttitude::Type AXwAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
    if (const APawn* OtherPawn = Cast<APawn>(&Other)) 
    {
		if (const ILyraTeamAgentInterface* TeamAgent = Cast<ILyraTeamAgentInterface>(OtherPawn->GetController()))
		{
			const FGenericTeamId OtherTeamID = TeamAgent->GetGenericTeamId();

			//Checking Other pawn ID to define Attitude
			if (OtherTeamID.GetId() != GetGenericTeamId().GetId())
			{
				return ETeamAttitude::Hostile;
			}
			else
			{
				return ETeamAttitude::Friendly;
			}
		}
	}

	return ETeamAttitude::Neutral;
}

void AXwAIController::HandleTargetPerceptionInfoUpdated(const FActorPerceptionUpdateInfo& InUpdateInfo)
{

	auto CurrentTiggerSense = UAIPerceptionSystem::GetSenseClassForStimulus(GetWorld(), InUpdateInfo.Stimulus);

	static FName ControllerName(TEXT("XwAIController"));

	if(CurrentTiggerSense == UAISense_Sight::StaticClass())
	{	
		if(InUpdateInfo.Stimulus.WasSuccessfullySensed())
		{
			if(!(GetPawn<AXwAICharacter>()->XpAIBag.FindPropertyDescByName(ControllerName)))
			{
				GetStateTreeAIComp()->SendStateTreeEvent(XpWorldTags::ST_State_FoundHostile, FConstStructView(), ControllerName);
				GetPawn<AXwAICharacter>()->XpAIBag.AddProperty(ControllerName, EPropertyBagPropertyType::Bool);
				return;
			}	
		}
		else
		{
			return;
		}		
	}

	GetStateTreeAIComp()->SendStateTreeEvent(XpWorldTags::ST_State_AttackHostile, FConstStructView(), ControllerName);
}
