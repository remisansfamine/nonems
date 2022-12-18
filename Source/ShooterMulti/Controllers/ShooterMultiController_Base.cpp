// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterMultiController_Base.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "GameFramework/GameModeBase.h"
#include "ShooterMulti/GameFramework/ShooterMultiGM_Base.h"

void AShooterMultiController_Base::SR_LaunchGame_Implementation()
{
	AShooterMultiGM_Base* GM = GetWorld()->GetAuthGameMode<AShooterMultiGM_Base>();
	GM->LaunchGame();
}

void AShooterMultiController_Base::SR_ReturnToLobby_Implementation()
{
	AShooterMultiGM_Base* GM = GetWorld()->GetAuthGameMode<AShooterMultiGM_Base>();
	GM->ReturnToLobby();
}

void AShooterMultiController_Base::CL_RemoveWidgets_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Removing widgets"));
	UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());
}

const FClientSetup AShooterMultiController_Base::GetClientSetup()
{
	AShooterMultiPS_Base* PS = GetPlayerState<AShooterMultiPS_Base>();

	if (PS)
		return PS->ClientSetup;

	return FClientSetup();
}

bool AShooterMultiController_Base::IsReady()
{
	AShooterMultiPS_Base* PS = GetPlayerState<AShooterMultiPS_Base>();
	
	if (PS)
		return PS->ClientSetup.bIsReady;

	return false;
}

bool AShooterMultiController_Base::IsHost()
{
	AShooterMultiPS_Base* PS = GetPlayerState<AShooterMultiPS_Base>();
	
	if (PS)
		return PS->ClientSetup.bIsHost;

	return false;
}

bool AShooterMultiController_Base::HasTeam()
{
	AShooterMultiPS_Base* PS = GetPlayerState<AShooterMultiPS_Base>();
	
	if (PS)
		return PS->ClientSetup.Team != ETeam::None;

	return false;
}

