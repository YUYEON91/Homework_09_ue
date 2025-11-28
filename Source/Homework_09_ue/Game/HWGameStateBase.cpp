// Fill out your copyright notice in the Description page of Project Settings.
// HWGameStateBase.cpp


#include "Game/HWGameStateBase.h"

#include "Kismet/GameplayStatics.h"
#include "Player/HWPlayerController.h"

void AHWGameStateBase::MulticastPRCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	if (HasAuthority() == false)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PC) == true)
		{
			AHWPlayerController* HWPC = Cast<AHWPlayerController>(PC);
			if (IsValid(HWPC) == true)
			{
				FString NotificationString = InNameString + TEXT(" (이)가 게임에 참가 하였습니다.");
				HWPC->PrintChatMessageString(NotificationString);
			}
		}
	}
}
