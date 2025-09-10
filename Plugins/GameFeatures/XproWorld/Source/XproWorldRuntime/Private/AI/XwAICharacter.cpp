// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/XwAICharacter.h"
#include "AbilitySystem/Attributes/LyraCombatSet.h"
#include "AbilitySystem/Attributes/LyraHealthSet.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Character/LyraPawnData.h" // @Game-Change 
#include "Character/LyraPawnExtensionComponent.h" // @Game-Change 
#include "AbilitySystem/LyraAbilitySet.h" // @Game-Change 
#include "Components/GameFrameworkComponentManager.h"  // @Game-Change 
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystem/Attribute/XpWorldMoveSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(XwAICharacter)

// @Game-Change name for the stage when we're ready for the abilities
const FName AXwAICharacter::NAME_LyraAbilityReady("LyraAbilitiesReady");

AXwAICharacter::AXwAICharacter(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
    AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<ULyraAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	AutoPossessAI = EAutoPossessAI::Spawned;

    // These attribute sets will be detected by AbilitySystemComponent::InitializeComponent. Keeping a reference so that the sets don't get garbage collected before that.
	HealthSet = CreateDefaultSubobject<ULyraHealthSet>(TEXT("HealthSet"));
	CombatSet = CreateDefaultSubobject<ULyraCombatSet>(TEXT("CombatSet"));

    SetNetUpdateFrequency(100.0f);
}

void AXwAICharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

UAbilitySystemComponent* AXwAICharacter::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void AXwAICharacter::SetPawnData(const ULyraPawnData* InPawnData)
{
	check(InPawnData);

	for (const ULyraAbilitySet* AbilitySet : InPawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_LyraAbilityReady);
}

void AXwAICharacter::OnAbilitySystemInitialized()
{
	Super::OnAbilitySystemInitialized();

    if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (const ULyraPawnExtensionComponent* LyraPawnExtensionComponent = ULyraPawnExtensionComponent::FindPawnExtensionComponent(this))
	{
		SetPawnData(LyraPawnExtensionComponent->GetPawnData<ULyraPawnData>());
	}
}

void AXwAICharacter::OnDeathStarted(AActor* OwningActor)
{
	Super::OnDeathStarted(OwningActor);

	// BeforeDestory(OwningActor);
}

void AXwAICharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UXpWorldMoveSet::GetMoveSpeedAttribute()).AddUObject(this, &ThisClass::HandleAttributeChanged);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UXpWorldMoveSet::GetAccelerationSpeedAttribute()).AddUObject(this, &ThisClass::HandleAttributeChanged);
}

void AXwAICharacter::UnPossessed()
{
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UXpWorldMoveSet::GetMoveSpeedAttribute()).RemoveAll(this);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UXpWorldMoveSet::GetAccelerationSpeedAttribute()).RemoveAll(this);
	Super::UnPossessed();
}

void AXwAICharacter::HandleAttributeChanged(const FOnAttributeChangeData& Data)
{
	if(Data.Attribute == UXpWorldMoveSet::GetMoveSpeedAttribute())
	{
		GetCharacterMovement()->MaxWalkSpeed = (1.f + (Data.NewValue / 100.f)) * Cast<ACharacter>(GetClass()->GetDefaultObject())->GetCharacterMovement()->MaxWalkSpeed;
	}
	else if(Data.Attribute == UXpWorldMoveSet::GetAccelerationSpeedAttribute())
	{
		GetCharacterMovement()->MaxAcceleration = (1.f + (Data.NewValue / 100.f)) * Cast<ACharacter>(GetClass()->GetDefaultObject())->GetCharacterMovement()->MaxAcceleration;
	}
}
