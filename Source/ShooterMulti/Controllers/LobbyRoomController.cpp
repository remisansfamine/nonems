// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyRoomController.h"

#include "GameFramework/GameMode.h"
#include "ShooterMulti/GameFramework/LobbyRoomGM.h"

void ALobbyRoomController::Sr_GetAndVerifyPassword_Implementation(const FString& password)
{
	ALobbyRoomGM* GM =  GetWorld()->GetAuthGameMode<ALobbyRoomGM>();

	
	
	GM->CheckUserPassword(password, this);
}
