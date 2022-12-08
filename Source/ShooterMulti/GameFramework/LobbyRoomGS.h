// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "LobbyRoomGS.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERMULTI_API ALobbyRoomGS : public AGameState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> LobbyCharacter;

	TArray<AActor*> GetSortedPlayerStarts() const;
	FTransform GetFreePlayerStart() const;
	FTransform GetFreePlayerStart(const TArray<AActor*>& PlayerStarts) const;
	
public:
	UFUNCTION(BlueprintCallable)
	void SpawnAllCharacters(const APlayerController* LocalController) const;
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void Multi_UpdateCharacters(const APlayerController* NewController);
};
