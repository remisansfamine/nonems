// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuPS.h"

#include "PlayerGI.h"

void AMainMenuPS::SetNameInstance()
{
	UPlayerGI* GameInstance = GetWorld()->GetGameInstance<UPlayerGI>();

	GameInstance->UserName = ClientSetup.Name;
}