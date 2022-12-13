// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterMultiPS_Base.h"

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
			BasePlayerState->ClientSetup = ClientSetup;
		}
	}
}

void AShooterMultiPS_Base::OverrideWith(APlayerState* PlayerState)
{
	Super::OverrideWith(PlayerState);

	PRINT("TryingToOverride in base");
	UE_LOG(LogTemp, Warning, TEXT("TryingToOverride in Base"))

	if (PlayerState)
	{
		UE_LOG(LogTemp, Warning, TEXT("TryingToOverride in Base2"))

		const AShooterMultiPS_Base* BasePlayerState = Cast<AShooterMultiPS_Base>(PlayerState);

		if (BasePlayerState)
		{
			ClientSetup = BasePlayerState->ClientSetup;
			PRINT("Override");
			UE_LOG(LogTemp, Warning, TEXT("Override"))
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

