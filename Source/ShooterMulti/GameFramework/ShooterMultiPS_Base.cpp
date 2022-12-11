// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterMultiPS_Base.h"

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5, FColor::White,text)

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
			BasePlayerState->UserName = UserName;
			BasePlayerState->ClientLocalName = ClientLocalName;
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

		}
	}
}

