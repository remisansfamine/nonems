// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShooterMultiGM_Base.h"
#include "LobbyRoomGM.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERMULTI_API ALobbyRoomGM : public AShooterMultiGM_Base
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TMap<FString, UTexture2D*> MapPreviewsDico;

	UPROPERTY()
	TArray<FString> MapNames;

public:
	UFUNCTION(BlueprintCallable)
	void OnMapSelectionChange(int dir);
	
	UFUNCTION(BlueprintImplementableEvent)
	void CheckUserPassword(const FString& password, const APlayerController* controller);
	
	UFUNCTION(BlueprintCallable)
	bool ParseServerConfig(FServerConfig& config);

	UFUNCTION(BlueprintImplementableEvent)
	void SetMaxScore(const int Score);

	UFUNCTION(BlueprintImplementableEvent)
	void SetMaxTime(const int Time);
	
	UFUNCTION(BlueprintCallable)
	void SetMaxScoreInInstance(const int Score);
	
	UFUNCTION(BlueprintCallable)
	void SetMaxTimeInInstance(const int Time);
	
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	virtual void BeginPlay() override;
};
