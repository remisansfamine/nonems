// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShooterMultiPS_Base.h"
#include "../Characters/LobbyRoomCharacter.h"
#include "LobbyRoomPS.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerSetReady, bool, IsReady);

/**
 * 
 */
UCLASS()
class SHOOTERMULTI_API ALobbyRoomPS : public AShooterMultiPS_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	ALobbyRoomCharacter* Pawn;

	UFUNCTION(BlueprintCallable)
	FString& GetClientSetup();

	UFUNCTION()
	void SetPawn();
	
	UPROPERTY(BlueprintAssignable)
	FServerSetReady OnServerSetReady;
	
	virtual void Rep_ClientDatas() override;
};
