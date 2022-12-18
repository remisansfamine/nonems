// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ShooterMultiGM_Base.generated.h"


USTRUCT(BlueprintType)
struct FServerConfig
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Name = "";

	UPROPERTY(BlueprintReadWrite)
	FString Password = "";

	UPROPERTY(BlueprintReadWrite)
	FString UUID = "";
	
	UPROPERTY(BlueprintReadWrite)
	int MaxSlots = 5;
};

/**
 * 
 */
UCLASS()
class SHOOTERMULTI_API AShooterMultiGM_Base : public AGameMode
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TArray<APlayerController*> Controllers;
	
	UFUNCTION(BlueprintNativeEvent)
	void LaunchGame();
	
	UFUNCTION(BlueprintNativeEvent)
	void ReturnToLobby();

	UFUNCTION(BlueprintCallable)
	void RemovePreviousWidgets();
};
