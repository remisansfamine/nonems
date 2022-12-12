// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShooterMultiPS_Base.h"
#include "ShooterMulti/Characters/LobbyRoomCharacter.h"
#include "LobbyRoomPS.generated.h"

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
	
	virtual void Rep_ClientDatas() override;
};
