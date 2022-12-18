// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterMultiPS_Base.h"

#include "MainMenuPS.h"
#include "ShooterPS.h"
#include "UtilsFunctionsLibrary.h"
#include "Net/UnrealNetwork.h"

void AShooterMultiPS_Base::BeginPlay()
{
	Super::BeginPlay();
}

void AShooterMultiPS_Base::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
	
	if (PlayerState)
	{
		AShooterMultiPS_Base* BasePlayerState = Cast<AShooterMultiPS_Base>(PlayerState);
		if (BasePlayerState)
		{
			ClientSetup.bIsReady = false;
			BasePlayerState->ClientSetup = ClientSetup;
		}

		AMainMenuPS* MenuPlayerState = Cast<AMainMenuPS>(PlayerState);
		if (MenuPlayerState)
		{
			MenuPlayerState->ClientSetup.Team = ETeam::None;
			MenuPlayerState->ClientSetup.bIsHost = false;
		}
	}
}

void AShooterMultiPS_Base::OverrideWith(APlayerState* PlayerState)
{
	Super::OverrideWith(PlayerState);

	if (PlayerState)
	{
		const AShooterMultiPS_Base* BasePlayerState = Cast<AShooterMultiPS_Base>(PlayerState);

		if (BasePlayerState)
		{
			ClientSetup = BasePlayerState->ClientSetup;
		}
	}
}

void AShooterMultiPS_Base::SR_SetClientSetup_Implementation(const FClientSetup& InSetup)
{
	ClientSetup = InSetup;
}

void AShooterMultiPS_Base::Rep_ClientDatas()
{
}

void AShooterMultiPS_Base::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AShooterMultiPS_Base, ClientSetup);
}

