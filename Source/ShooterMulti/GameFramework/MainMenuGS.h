// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "ShooterMultiPS_Base.h"
#include "MainMenuGS.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERMULTI_API AMainMenuGS : public AGameState
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	bool LoadClientGameSetup(FClientSetup& ClientSetup, const FString& ClientIdentifier);

	UFUNCTION(BlueprintCallable)
	bool SaveClientGameSetup(const FClientSetup& ClientSetup, const FString& ClientIdentifier);
};
