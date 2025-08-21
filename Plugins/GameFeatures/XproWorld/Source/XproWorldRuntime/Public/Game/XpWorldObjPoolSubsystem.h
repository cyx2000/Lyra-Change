// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "Blueprint/UserWidgetPool.h"
#include "XpWorldObjPoolSubsystem.generated.h"

struct FActorSpawnParameters;

USTRUCT(BlueprintType)
struct FXpWorldActorPool
{
	GENERATED_BODY()

public:

	FXpWorldActorPool(){};
	void SetWorld(UWorld* World)
	{
		if(World)
		{
			OwningWorld = World;
		}
	}
	bool IsInitialized() const { return OwningWorld.IsValid(); }

	AActor* GetActor(TSubclassOf<AActor> ActorClass);

	//创建actor
	template <typename ActorT = AActor>
	AActor* GetOrCreateActor(TSubclassOf<ActorT> ActorClass, const FVector& Location, const FRotator& Rotation, const FActorSpawnParameters& SpawnParameters)
	{
		if (!ActorClass || !ensure(IsInitialized()))
		{
			return nullptr;
		}

		AActor* Actor = nullptr;
		for (AActor* InactiveActor : InactiveActors)
		{
			Actor = InactiveActor;
			InactiveActors.RemoveSingleSwap(InactiveActor);
			break;
		}
		if(Actor)
		{
			SetActorEnable(Actor,true);
		}
		if (!Actor)
		{
			Actor = OwningWorld.Get()->SpawnActor(ActorClass, &Location, &Rotation, SpawnParameters);
			if (!Actor)
			{
				return nullptr;
			}
		}
		ActiveActors.Add(Actor);

		return Cast<ActorT>(Actor);
	}

	//将actor释放回对象池
	void ReleaseActor(AActor* Actor);

	void ReleaseActors(TArray<AActor*> Actors);

	void ReleaseAll();

	//重置对象池
	void ResetPool();

	//设置Actor的开启与关闭（碰撞、显示、Tick和所有子Actor）
	static void SetActorEnable(AActor* Actor, bool Enable);;

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<AActor>> ActiveActors;
	UPROPERTY(Transient)
	TArray<TObjectPtr<AActor>> InactiveActors;

	TWeakObjectPtr<UWorld> OwningWorld;
};

/**
 * 
 */
UCLASS()
class UXpWorldObjPoolSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	//创建Widget
	UFUNCTION(BlueprintCallable)
	XPROWORLDRUNTIME_API UUserWidget* CreateOrGetWidget(TSubclassOf<UUserWidget> WidgetClass);
	//释放Widget到对象池
	UFUNCTION(BlueprintCallable)
	XPROWORLDRUNTIME_API void ReleaseWidget(UUserWidget* Widget);
	UFUNCTION(BlueprintCallable)
	XPROWORLDRUNTIME_API void ReleaseWidgets(TArray<UUserWidget*> Widgets);

	//创建Actor
	UFUNCTION(BlueprintCallable)
	XPROWORLDRUNTIME_API AActor* CreateOrGetActor(TSubclassOf<AActor> ActorClass, 
		FVector Location=FVector::ZeroVector, 
		FRotator Rotation=FRotator::ZeroRotator);

	AActor* CreateOrGetActorInternal(TSubclassOf<AActor> ActorClass, 
		FVector Location=FVector::ZeroVector, 
		FRotator Rotation=FRotator::ZeroRotator, 
		const FActorSpawnParameters& SpawnParameters = FActorSpawnParameters());

	//释放Actor到对象池
	UFUNCTION(BlueprintCallable)
	XPROWORLDRUNTIME_API void ReleaseActor(AActor* Actor);
	UFUNCTION(BlueprintCallable)
	XPROWORLDRUNTIME_API void ReleaseActors(TArray<AActor*> Actors);
	UFUNCTION(BlueprintCallable)
	XPROWORLDRUNTIME_API void ReleaseActorByClass(TSubclassOf<AActor> ActorClass);
	UFUNCTION(BlueprintCallable)
	XPROWORLDRUNTIME_API void ResetPool(TSubclassOf<AActor> ActorClass);
	
protected:
	virtual void PostInitialize() override;

private:
	UPROPERTY()
	FUserWidgetPool WidgetPool;
	UPROPERTY()
	TMap<TSubclassOf<AActor>, FXpWorldActorPool> ActorPool;
	
};
