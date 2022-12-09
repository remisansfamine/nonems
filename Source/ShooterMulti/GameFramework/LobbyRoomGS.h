// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "LobbyRoomGS.generated.h"

UCLASS()
class SHOOTERMULTI_API ALobbyRoomGS : public AGameState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> LobbyCharacter;

	UPROPERTY()
	class APlayerStart* SelfPlayerStart;
	
	UPROPERTY()
	TArray<class APlayerStart*> PlayerStarts;

	UPROPERTY()
	TArray<ACharacter*> CharactersSpawned;

	UPROPERTY()
	APlayerController* SelfController;

	FTransform GetFreePlayerStart() const;

	void SetupPlayerStart();
	
public:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	void SpawnSelfCharacter(APlayerController* LocalController);

	UFUNCTION(BlueprintCallable)
	void UpdateCharacters();
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void Multi_UpdateCharacters(const APlayerController* NewController);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
