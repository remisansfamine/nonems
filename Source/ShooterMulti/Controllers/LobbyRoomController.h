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

	FTransform GetFreePlayerStart(TArray<AActor*> PlayerStarts) const;

public:

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> LobbyCharacter;
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Sr_GetAndVerifyPassword(const FString& password);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void CL_UpdateCharacters(int characterCount);
};
