// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "StateTreeTaskBase.h"
#include "XwSTTaskBase.generated.h"

USTRUCT(BlueprintType)
struct FXwSTTaskMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bTaskSuccess;

};


USTRUCT(meta = (Hidden))
struct FXwSTTaskBase : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

};



