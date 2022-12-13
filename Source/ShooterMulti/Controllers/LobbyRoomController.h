// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
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

	UFUNCTION(Server, Reliable)
	void SR_SetReady();

	UFUNCTION(BlueprintCallable)
	bool SetReady();
	
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void CL_SpawnAllCharacters();
};
