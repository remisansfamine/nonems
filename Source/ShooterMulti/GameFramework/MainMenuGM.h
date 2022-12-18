// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShooterMultiGM_Base.h"
#include "MainMenuGM.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERMULTI_API AMainMenuGM : public AGameMode
{
	GENERATED_BODY()

private:
	static FString GetServerDefaultMapName();
	static FString FormatCommandParameters();
	
public:
	UFUNCTION(BlueprintCallable)
	void LaunchServerInstance();

	UFUNCTION(BlueprintCallable)
	bool SaveServerConfig(const FServerConfig& config);
};
