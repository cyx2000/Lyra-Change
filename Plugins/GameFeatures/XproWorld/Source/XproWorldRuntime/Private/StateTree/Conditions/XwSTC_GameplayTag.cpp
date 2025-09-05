#include "StateTree/Conditions/XwSTC_GameplayTag.h"
#include "GameplayTagContainer.h"
#include "Misc/AssertionMacros.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeNodeDescriptionHelpers.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(XwSTC_GameplayTag)


bool FXwGASTagsMatchCondition::TestCondition(FStateTreeExecutionContext& Context) const
{
    const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

    check(Cast<IAbilitySystemInterface>(InstanceData.Actor));
    
    const FGameplayTagContainer& GASTags = Cast<IAbilitySystemInterface>(InstanceData.Actor)->GetAbilitySystemComponent()->GetOwnedGameplayTags();

    bool bResult = false;
	switch (MatchType)
	{
	case EGameplayContainerMatchType::Any:
		bResult = bExactMatch ? GASTags.HasAnyExact(InstanceData.TagContainer) : GASTags.HasAny(InstanceData.TagContainer);
		break;
	case EGameplayContainerMatchType::All:
		bResult = bExactMatch ? GASTags.HasAllExact(InstanceData.TagContainer) : GASTags.HasAll(InstanceData.TagContainer);
		break;
	default:
		ensureMsgf(false, TEXT("Unhandled match type %s."), *UEnum::GetValueAsString(MatchType));
	}

    return bResult ^ bInvert;
}
