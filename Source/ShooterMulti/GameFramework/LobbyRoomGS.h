// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "ShooterMulti/Characters/LobbyRoomCharacter.h"
#include "LobbyRoomGS.generated.h"

USTRUCT(BlueprintType)
struct FMapPreviewSetup
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Name = "";

	UPROPERTY(BlueprintReadWrite)
	FString Path = "";
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMapChangeDelegate);

UCLASS()
class SHOOTERMULTI_API ALobbyRoomGS : public AGameState
{
	GENERATED_BODY()

	// Blueprint instance of the lobby character
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> LobbyCharacter;

	// Player start for the owner (local client)
	UPROPERTY()
	class APlayerStart* SelfPlayerStart;

	// Player start available for the other clients
	UPROPERTY()
	TArray<class APlayerStart*> RightPlayerStarts;
	UPROPERTY()
	TArray<class APlayerStart*> LeftPlayerStarts;
	UPROPERTY()
	TArray<class APlayerStart*> NonePlayerStarts;
	
	// All client characters spawned
	UPROPERTY()
	TArray<ACharacter*> CharactersSpawned;

	bool bClientHasSwitchTeam = false;
	
	void SetupPlayerStart();
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnMapChangeDelegate MapChangeDelegate;
	
	void SetNewCurrentMap(const FString& MapName, const FString& MapPath);
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void Multi_OnSwitchTeam();

	UFUNCTION(BlueprintCallable)
	void OnSwitchTeam();

	int MapIndex = 0;

	UPROPERTY(ReplicatedUsing=OnRep_SetNewCurrentMap)
	FMapPreviewSetup CurrentMapSetup;

	UPROPERTY(BlueprintReadOnly)
	FString CurrentMapPreviewName;
	
	UPROPERTY(BlueprintReadOnly)
	UTexture2D* CurrentMapPreviewTexture;

	UFUNCTION()
	void OnRep_SetNewCurrentMap();
	
	// Local player controller
	UPROPERTY()
	APlayerController* SelfController;

	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	void SpawnSelfCharacter(APlayerController* LocalController);

	UFUNCTION(BlueprintCallable)
	void UpdateCharacters();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
