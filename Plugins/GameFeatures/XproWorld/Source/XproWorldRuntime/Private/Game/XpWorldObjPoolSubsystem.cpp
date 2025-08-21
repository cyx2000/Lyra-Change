// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/XpWorldObjPoolSubsystem.h"
#include "CoreGlobals.h"
#include "HAL/Platform.h"
#include "Logging/LogMacros.h"
#include "Logging/LogVerbosity.h"
#include "Math/MathFwd.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(XpWorldObjPoolSubsystem)

AActor* FXpWorldActorPool::GetActor(TSubclassOf<AActor> ActorClass)
{
	return GetOrCreateActor(ActorClass, FVector::ZeroVector, FRotator::ZeroRotator, FActorSpawnParameters());
}

void FXpWorldActorPool::ReleaseActor(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}
	int32 WantIndex;
	if (ActiveActors.Find(Actor, WantIndex))
	{
		ActiveActors.RemoveAt(WantIndex);
		InactiveActors.Push(Actor);
		SetActorEnable(Actor,false);
	}
}

void FXpWorldActorPool::ReleaseActors(TArray<AActor*> Actors)
{
	for (AActor* Actor : Actors)
	{
		ReleaseActor(Actor);
	}
}

void FXpWorldActorPool::ReleaseAll()
{
	for(auto Actor:ActiveActors)
	{
		SetActorEnable(Actor,false);
	}
	InactiveActors.Append(ActiveActors);
	ActiveActors.Empty();
}

void FXpWorldActorPool::ResetPool()
{
	InactiveActors.Reset();
	ActiveActors.Reset();
}

void FXpWorldActorPool::SetActorEnable(AActor* Actor, bool Enable)
{
	if(!Actor)
		return;

	Actor->SetActorHiddenInGame(!Enable);
	Actor->SetActorEnableCollision(Enable);
	// Actor->SetActorTickEnabled(Enable);

	TArray<UActorComponent*> Components;
	Actor->GetComponents(Components);
	for (UActorComponent* ActorComponent : Components)
	{
		ActorComponent->SetActive(Enable);
		// ActorComponent->SetComponentTickEnabled(Enable);
	}
}

void UXpWorldObjPoolSubsystem::PostInitialize()
{
	Super::PostInitialize();
	WidgetPool.SetWorld(GetWorld());
	//ActorPool.SetWorld(GetWorld());
}

UUserWidget* UXpWorldObjPoolSubsystem::CreateOrGetWidget(const TSubclassOf<UUserWidget> WidgetClass)
{
	return WidgetPool.GetOrCreateInstance(WidgetClass);
}

void UXpWorldObjPoolSubsystem::ReleaseWidget(UUserWidget* Widget)
{
	if(!Widget) return;
	Widget->SetVisibility(ESlateVisibility::Collapsed);
	WidgetPool.Release(Widget,false);
}

void UXpWorldObjPoolSubsystem::ReleaseWidgets(TArray<UUserWidget*> Widgets)
{
	for (const auto Widget : Widgets)
	{
		ReleaseWidget(Widget);
	}
}

AActor* UXpWorldObjPoolSubsystem::CreateOrGetActor(TSubclassOf<AActor> ActorClass, 
		FVector Location, FRotator Rotation)
{
	return CreateOrGetActorInternal(ActorClass, Location, Rotation);
}

AActor* UXpWorldObjPoolSubsystem::CreateOrGetActorInternal(TSubclassOf<AActor> ActorClass, 
	FVector InLocation, 
	FRotator InRotation, 
	const FActorSpawnParameters& InSpawnParameters)
{
	check(ActorClass);
	
	if(const auto Pool = ActorPool.Find(ActorClass))
	{
		return Pool->GetOrCreateActor(ActorClass, InLocation, InRotation, InSpawnParameters);
	}
	else
	{
		FXpWorldActorPool NewPool;
		NewPool.SetWorld(GetWorld());
		ActorPool.Add(ActorClass, NewPool);

		return NewPool.GetOrCreateActor(ActorClass, InLocation, InRotation, InSpawnParameters);
	}
}

void UXpWorldObjPoolSubsystem::ReleaseActor(AActor* Actor)
{
	if(!Actor) return;

	if(const auto Pool = ActorPool.Find(Actor->GetClass()))
	{
		Pool->ReleaseActor(Actor);
	}
}

void UXpWorldObjPoolSubsystem::ReleaseActors(TArray<AActor*> Actors)
{
	if(!Actors.IsEmpty())
	{
		if(const auto Pool = ActorPool.Find(Actors.Last()->GetClass()))
		{
			Pool->ReleaseActors(Actors);
		}
	}
}

void UXpWorldObjPoolSubsystem::ReleaseActorByClass(TSubclassOf<AActor> ActorClass)
{
	if(const auto Pool = ActorPool.Find(ActorClass))
	{
		Pool->ReleaseAll();
	}
}

void UXpWorldObjPoolSubsystem::ResetPool(TSubclassOf<AActor> ActorClass)
{
	if(const auto Pool = ActorPool.Find(ActorClass))
	{
		Pool->ResetPool();
	}
}

