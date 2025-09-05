#pragma once

#include "StateTree/XwSTConditionBase.h"
#include "XwSTC_GameplayTag.generated.h"

USTRUCT()
struct FXwGASTagsMatchConditionInstanceData
{
	GENERATED_BODY()

	/** Container to check for the tag. */
	UPROPERTY(EditAnywhere, Category = Parameter)
	TObjectPtr<AActor> Actor;

	/** Tags to check for in the container. */
	UPROPERTY(EditAnywhere, Category = Parameter)
	FGameplayTagContainer TagContainer;
};

USTRUCT(DisplayName="GAS Has Tags", Category="Xw|Tags")
struct FXwGASTagsMatchCondition : public FXwSTConditionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FXwGASTagsMatchConditionInstanceData;

	FXwGASTagsMatchCondition() = default;
	
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	XPROWORLDRUNTIME_API virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
#if WITH_EDITOR
	virtual FColor GetIconColor() const override
	{
		return UE::StateTree::Colors::DarkGrey;
	}
#endif

	UPROPERTY(EditAnywhere, Category = Condition)
	EGameplayContainerMatchType MatchType = EGameplayContainerMatchType::Any;

	UPROPERTY(EditAnywhere, Category = Condition)
	bool bExactMatch = false;

	UPROPERTY(EditAnywhere, Category = Condition)
	bool bInvert = false;
};