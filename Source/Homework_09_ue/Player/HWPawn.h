// Fill out your copyright notice in the Description page of Project Settings.
// HWPawn.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "HWPawn.generated.h"

UCLASS()
class HOMEWORK_09_UE_API AHWPawn : public APawn
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

};
