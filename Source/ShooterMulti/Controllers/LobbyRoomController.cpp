// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyRoomController.h"

#include "GameFrameWork/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "ShooterMulti/GameFramework/LobbyRoomGM.h"
#include "ShooterMulti/GameFramework/LobbyRoomGS.h"

void ALobbyRoomController::Sr_GetAndVerifyPassword_Implementation(const FString& password)
{
	ALobbyRoomGM* GM =  GetWorld()->GetAuthGameMode<ALobbyRoomGM>();

	if (GM)
		GM->CheckUserPassword(password, this);
}

void ALobbyRoomController::CL_SpawnAllCharacters_Implementation()
{
	ALobbyRoomGS* GS = GetWorld()->GetGameState<ALobbyRoomGS>();

	if (GS)
		GS->SpawnAllCharacters(this);
}