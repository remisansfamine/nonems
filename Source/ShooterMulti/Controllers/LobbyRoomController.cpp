// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyRoomController.h"

#include "UtilsFunctionsLibrary.h"
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

void ALobbyRoomController::SR_SetHost_Implementation(const bool IsHost)
{
	ALobbyRoomPS* PS = GetPlayerState<ALobbyRoomPS>();

	if (PS)
		PS->ClientSetup.bIsHost = IsHost;
}

void ALobbyRoomController::SR_SendMaxScore_Implementation(const int Score)
{
	ALobbyRoomGM* GameMode = GetWorld()->GetAuthGameMode<ALobbyRoomGM>();
	GameMode->SetMaxScore(Score);
}

void ALobbyRoomController::SR_SendMaxTime_Implementation(const int Time)
{
	ALobbyRoomGM* GameMode = GetWorld()->GetAuthGameMode<ALobbyRoomGM>();
	GameMode->SetMaxTime(Time);
}

bool ALobbyRoomController::IsReady()
{
	ALobbyRoomPS* PS = GetPlayerState<ALobbyRoomPS>();
	
	if (PS)
		return PS->ClientSetup.bIsReady;

	return false;
}

bool ALobbyRoomController::IsHost()
{
	ALobbyRoomPS* PS = GetPlayerState<ALobbyRoomPS>();
	
	if (PS)
		return PS->ClientSetup.bIsHost;

	return false;
}

const FClientSetup ALobbyRoomController::GetClientSetup()
{
	ALobbyRoomPS* PS = GetPlayerState<ALobbyRoomPS>();

	if (PS)
		return PS->ClientSetup;

	return FClientSetup();
}

FClientSetup ALobbyRoomController::SetClientSetup(const FString& InName, const ETeam& InTeam)
{
	ALobbyRoomPS* PS = GetPlayerState<ALobbyRoomPS>();
	
	if (PS)
	{
		PS->ClientSetup.Name = InName;
		PS->ClientSetup.Team = InTeam;
		
		return PS->ClientSetup;
	}

	return FClientSetup();
}

bool ALobbyRoomController::SetReady()
{
	if (GetLocalRole() != ROLE_AutonomousProxy)
		return false;
	
	ALobbyRoomPS* PS = GetPlayerState<ALobbyRoomPS>();

	if (PS)
		if (PS->ClientSetup.Team == ETeam::None)
			return false;
	
	SR_SetReady(true);
	return true;
}

bool ALobbyRoomController::CancelReady()
{
	if (GetLocalRole() != ROLE_AutonomousProxy)
		return false;

	SR_SetReady(false);
	return true;
}

void ALobbyRoomController::SR_SetReady_Implementation(const bool NewState)
{
	ALobbyRoomPS* PS = GetPlayerState<ALobbyRoomPS>();

	if (PS)
		PS->ClientSetup.bIsReady = NewState;
}
