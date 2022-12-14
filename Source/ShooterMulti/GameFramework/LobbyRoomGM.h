// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyRoomGM.generated.h"

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
class SHOOTERMULTI_API ALobbyRoomGM : public AGameMode
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void CheckUserPassword(const FString& password, const APlayerController* controller);
	
	UFUNCTION(BlueprintCallable)
	bool ParseServerConfig(FServerConfig& config);

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
};
