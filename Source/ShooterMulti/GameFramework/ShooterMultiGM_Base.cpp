// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterMultiGM_Base.h"
#include "ShooterMulti/Controllers/ShooterMultiController_Base.h"

void AShooterMultiGM_Base::LaunchGame_Implementation()
{
	RemovePreviousWidgets();
}

void AShooterMultiGM_Base::ReturnToLobby_Implementation()
{
	RemovePreviousWidgets();
}

void AShooterMultiGM_Base::RemovePreviousWidgets()
{
	for (const auto PController : Controllers)
	{
		AShooterMultiController_Base* ShooterController = Cast<AShooterMultiController_Base>(PController);
		ShooterController->CL_RemoveWidgets();
	}
}
