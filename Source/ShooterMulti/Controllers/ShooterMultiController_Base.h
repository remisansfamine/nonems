// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterMulti/GameFramework/ShooterMultiPS_Base.h"
#include "ShooterMultiController_Base.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERMULTI_API AShooterMultiController_Base : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SR_LaunchGame();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SR_ReturnToLobby();

	UFUNCTION(Client, Unreliable)
	void CL_RemoveWidgets();
	
	UFUNCTION(BlueprintCallable)
	const FClientSetup GetClientSetup();

	UFUNCTION(BlueprintCallable)
	bool IsReady();
	
	UFUNCTION(BlueprintCallable)
	bool IsHost();

	UFUNCTION(BlueprintCallable)
	bool HasTeam();
};
