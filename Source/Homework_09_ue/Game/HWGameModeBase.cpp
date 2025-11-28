// Fill out your copyright notice in the Description page of Project Settings.
// HWGameModeBase.cpp

#include "Game/HWGameModeBase.h"

#include "HWGameStateBase.h"
#include "Player/HWPlayerController.h"
#include "EngineUtils.h"
#include "Player/HWPlayerState.h"

void AHWGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	AHWPlayerController* HWPlayerController = Cast<AHWPlayerController>(NewPlayer);
	if (IsValid(HWPlayerController) == true)
	{
		HWPlayerController->NotificationText = FText::FromString(TEXT("게임 서버에 연결 되었습니다."));

		AllPlayerControllers.Add(HWPlayerController);

		AHWPlayerState* HWPS = HWPlayerController->GetPlayerState<AHWPlayerState>();
		if (IsValid(HWPS) == true)
		{
			HWPS->PlayerNameString = TEXT("플레이어") + FString::FromInt(AllPlayerControllers.Num());
		}

		AHWGameStateBase* HWGameStateBase = GetGameState<AHWGameStateBase>();
		if (IsValid(HWGameStateBase) == true)
		{
			HWGameStateBase->MulticastPRCBroadcastLoginMessage(HWPS->PlayerNameString);
		}
	}
}

FString AHWGameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0; });

	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	return Result;
}

bool AHWGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	bool bCanPlay = false;

	do {

		if (InNumberString.Len() != 3)
		{
			break;
		}

		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				break;
			}

			UniqueDigits.Add(C);
		}

		if (bIsUnique == false)
		{
			break;
		}

		bCanPlay = true;

	} while (false);

	return bCanPlay;
}

FString AHWGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("아웃");
	}

	return FString::Printf(TEXT("%d 스트라이크 %d 볼"), StrikeCount, BallCount);
}

void AHWGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SecretNumberString = GenerateSecretNumber();
	UE_LOG(LogTemp, Error, TEXT("%s"), *SecretNumberString);
}

void AHWGameModeBase::PrintChatMessageString(AHWPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	int Index = InChatMessageString.Len() - 3;
	FString GuessNumberString = InChatMessageString.RightChop(Index);
	if (IsGuessNumberString(GuessNumberString) == true)
	{
		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);

		IncreaseGuessCount(InChattingPlayerController);

		for (TActorIterator<AHWPlayerController> It(GetWorld()); It; ++It)
		{
			AHWPlayerController* HWPlayerController = *It;
			if (IsValid(HWPlayerController) == true)
			{
				FString CombinedMessageString = InChatMessageString + TEXT(" -> ") + JudgeResultString;
				HWPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);

				int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
				JudgeGame(InChattingPlayerController, StrikeCount);
			}
		}
	}
	else
	{
		for (TActorIterator<AHWPlayerController> It(GetWorld()); It; ++It)
		{
			AHWPlayerController* HWPlayerController = *It;
			if (IsValid(HWPlayerController) == true)
			{
				HWPlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
			}
		}
	}
}

void AHWGameModeBase::IncreaseGuessCount(AHWPlayerController* InChattingPlayerController)
{
	AHWPlayerState* HWPS = InChattingPlayerController->GetPlayerState<AHWPlayerState>();
	if (IsValid(HWPS) == true)
	{
		// 첫 입력은 바로 1로 시작
		if (HWPS->CurrentGuessCount == 0)
		{
			HWPS->CurrentGuessCount = 1;
		}
		else
		{
			HWPS->CurrentGuessCount++;
		}

		// 모든 플레이어가 3번 입력했는지 확인
		bool bAllPlayersReachedMax = true;

		for (AHWPlayerController* HWPlayerController : AllPlayerControllers)
		{
			if (!IsValid(HWPlayerController))
				continue;

			AHWPlayerState* OtherPS = HWPlayerController->GetPlayerState<AHWPlayerState>();
			if (!IsValid(OtherPS))
				continue;

			// 3번 미만 입력한 플레이어가 있다 → 아직 4번째 입력 조건 불가
			if (OtherPS->CurrentGuessCount < OtherPS->MaxGuessCount)
			{
				bAllPlayersReachedMax = false;
				break;
			}
		}

		// 모든 플레이어가 3번을 이미 입력한 상태에서, 이번 입력이 4번째 입력이면 ResetGame 실행
		if (bAllPlayersReachedMax && HWPS->CurrentGuessCount > HWPS->MaxGuessCount)
		{
			// 모든 플레이어에게 공지
			for (AHWPlayerController* HWPlayerController : AllPlayerControllers)
			{
				if (IsValid(HWPlayerController))
				{
					HWPlayerController->NotificationText =
						FText::FromString(TEXT("모든 플레이어가 3회 입력을 완료했습니다. 4번째 입력으로 게임을 리셋합니다."));
				}
			}

			ResetGame();
		}
	}
}

void AHWGameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();

	for (const auto& HWPlayerController : AllPlayerControllers)
	{
		AHWPlayerState* HWPS = HWPlayerController->GetPlayerState<AHWPlayerState>();
		if (IsValid(HWPS) == true)
		{
			HWPS->CurrentGuessCount = 0;
		}
	}
}

void AHWGameModeBase::JudgeGame(AHWPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (3 == InStrikeCount)
	{
		AHWPlayerState* HWPS = InChattingPlayerController->GetPlayerState<AHWPlayerState>();
		for (const auto& HWPlayerController : AllPlayerControllers)
		{
			if (IsValid(HWPS) == true)
			{
				FString CombinedMessageString = HWPS->PlayerNameString + TEXT("(이)가 게임에서 이겼습니다.");
				HWPlayerController->NotificationText = FText::FromString(CombinedMessageString);

				ResetGame();
			}
		}
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& HWPlayerController : AllPlayerControllers)
		{
			AHWPlayerState* HWPS = HWPlayerController->GetPlayerState<AHWPlayerState>();
			if (IsValid(HWPS) == true)
			{
				if (HWPS->CurrentGuessCount < HWPS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}

		if (true == bIsDraw)
		{
			for (const auto& HWPlayerController : AllPlayerControllers)
			{
				HWPlayerController->NotificationText = FText::FromString(TEXT("비겼습니다..."));

				ResetGame();
			}
		}
	}
}
