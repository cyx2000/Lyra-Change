// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/XwAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Components/StateTreeAIComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(XwAIController)

AXwAIController::AXwAIController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
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
	UE_LOG(LogTemp, Warning, TEXT("HandleTargetPerceptionInfoUpdated: %d"), InUpdateInfo.TargetId);
}
