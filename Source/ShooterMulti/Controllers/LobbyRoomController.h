// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterMulti/GameFramework/ShooterMultiPS_Base.h"
#include "LobbyRoomController.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERMULTI_API ALobbyRoomController : public APlayerController
{
	GENERATED_BODY()

public:
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SR_GetAndVerifyPassword(const FString& password);

	// Getter and Setter of ClientSetup in Controller's Player State
	UFUNCTION(BlueprintCallable)
	const FClientSetup GetClientSetup();

	UFUNCTION(BlueprintCallable)
	FClientSetup SetClientSetup(const FString& InName, const ETeam& InTeam);
	
	UFUNCTION(Server, Reliable)
	void SR_SetReady(const bool NewState);
	
	UFUNCTION(Server, Unreliable, BlueprintCallable)
	void SR_SetHost(const bool IsHost);
	
	UFUNCTION(BlueprintCallable)
	bool IsReady();
	
	UFUNCTION(BlueprintCallable)
	bool IsHost();
	
	UFUNCTION(BlueprintCallable)
	bool SetReady();

	UFUNCTION(BlueprintCallable)
	bool CancelReady();

	
	UFUNCTION(Server, Unreliable, BlueprintCallable)
	void SR_SendMaxScore(const int Score);
	
	UFUNCTION(Server, Unreliable, BlueprintCallable)
	void SR_SendMaxTime(const int Time);
	
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void CL_SpawnAllCharacters();
};
