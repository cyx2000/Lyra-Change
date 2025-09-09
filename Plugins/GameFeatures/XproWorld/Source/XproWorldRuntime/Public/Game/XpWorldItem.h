#pragma once

#include "StructUtils/PropertyBag.h"
#include "GameplayEffectTypes.h"
#include "XpWorldItem.generated.h"

class UStaticMesh;
class UXWorldInventoryItemDefinition;

USTRUCT(BlueprintType)
struct XPROWORLDRUNTIME_API FXpWorldItemVisi : public FTableRowBase
{
	GENERATED_BODY()

public:
	// The message to display
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;

    // The message to display
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayDesc;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UStaticMesh> DisplayMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayTagContainer ItemTags;
};

USTRUCT(BlueprintType)
struct XPROWORLDRUNTIME_API FXpWorldConutryItemEntry
{
	GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<UXWorldInventoryItemDefinition> ItemDef;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Count = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Weight;
};

USTRUCT(BlueprintType)
struct XPROWORLDRUNTIME_API FXpWorldConutryItems : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float NonePercent = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 NumToSpawn = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FXpWorldConutryItemEntry> Items;
};

USTRUCT(BlueprintType)
struct XPROWORLDRUNTIME_API FXpWorldInstanceBag : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = Common)
	FInstancedPropertyBag XpBag;
};