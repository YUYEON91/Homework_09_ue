// Fill out your copyright notice in the Description page of Project Settings.
// HWGameStateBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "HWGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class HOMEWORK_09_UE_API AHWGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPRCBroadcastLoginMessage(const FString & InNameString = FString(TEXT("XXXXXX")));

};
