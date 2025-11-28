// Fill out your copyright notice in the Description page of Project Settings.
// HWGameModeBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HWGameModeBase.generated.h"

class AHWPlayerController;

/**
 * 
 */
UCLASS()
class HOMEWORK_09_UE_API AHWGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void OnPostLogin(AController* NewPlayer) override;

	FString GenerateSecretNumber();

	bool IsGuessNumberString(const FString& InNumberString);

	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);

	virtual void BeginPlay() override;

	void PrintChatMessageString(AHWPlayerController* InChattingPlayerController, const FString& InChatMessageString);

	void IncreaseGuessCount(AHWPlayerController* InChattingPlayerController);

	void ResetGame();

	void JudgeGame(AHWPlayerController* InChattingPlayerController, int InStrikeCount);

protected:
	FString SecretNumberString;

	TArray<TObjectPtr<AHWPlayerController>> AllPlayerControllers;

};
