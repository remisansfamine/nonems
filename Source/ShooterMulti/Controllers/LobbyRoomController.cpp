// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyRoomController.h"

#include "GameFrameWork/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "ShooterMulti/GameFramework/LobbyRoomGM.h"
#include "ShooterMulti/GameFramework/LobbyRoomGS.h"
#include "ShooterMulti/GameFramework/LobbyRoomPS.h"

void ALobbyRoomController::SR_GetAndVerifyPassword_Implementation(const FString& password)
{
	ALobbyRoomGM* GM =  GetWorld()->GetAuthGameMode<ALobbyRoomGM>();

	if (GM)
		GM->CheckUserPassword(password, this);
}

void ALobbyRoomController::CL_SpawnAllCharacters_Implementation()
{
	ALobbyRoomGS* GS = GetWorld()->GetGameState<ALobbyRoomGS>();

	if (GS)
		GS->SpawnSelfCharacter(this);
}

bool ALobbyRoomController::SetReady()
{
	if (GetLocalRole() != ROLE_AutonomousProxy)
		return false;
	
	ALobbyRoomPS* PS = GetPlayerState<ALobbyRoomPS>();

	if (PS)
		if (PS->ClientSetup.Team == ETeam::None)
			return false;
	
	SR_SetReady();
	return true;
}

void ALobbyRoomController::SR_SetReady_Implementation()
{
	ALobbyRoomPS* PS = GetPlayerState<ALobbyRoomPS>();

	if (PS)
		PS->ClientSetup.bIsReady = true;
}
