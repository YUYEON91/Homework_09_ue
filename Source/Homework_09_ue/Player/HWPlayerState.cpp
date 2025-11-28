// Fill out your copyright notice in the Description page of Project Settings.
// HWPlayerState.cpp

#include "Player/HWPlayerState.h"

#include "Net/UnrealNetwork.h"

AHWPlayerState::AHWPlayerState()
	: PlayerNameString(TEXT("None"))
	, CurrentGuessCount(0)
	, MaxGuessCount(3)
{
	bReplicates = true;
}

void AHWPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayerNameString);
	DOREPLIFETIME(ThisClass, CurrentGuessCount);
	DOREPLIFETIME(ThisClass, MaxGuessCount);

}

FString AHWPlayerState::GetPlayerInfoString()
{
	// 첫 입력을 1로 카운트하기 위해 `CurrentGuessCount + 1'로 작성 
	FString PlayerInfoString = PlayerNameString + TEXT("(") + FString::FromInt(CurrentGuessCount + 1) + TEXT("/") + FString::FromInt(MaxGuessCount) + TEXT(")");
	return PlayerInfoString;
}
