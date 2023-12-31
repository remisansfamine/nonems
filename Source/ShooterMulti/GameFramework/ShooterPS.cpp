#include "ShooterPS.h"
#include "PlayerGI.h"
#include "../Characters/ShooterCharacter.h"
#include "DeathMatchGS.h"
#include "UtilsFunctionsLibrary.h"
#include "Net/UnrealNetwork.h"

void AShooterPS::BeginPlay()
{
	Super::BeginPlay();

	ADeathMatchGS* GameState = GetWorld()->GetGameState<ADeathMatchGS>();
	GameState->OnResetAfterDelay.AddLambda([this]() { Reset(); });
}

void AShooterPS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterPS, NbDeath);
	DOREPLIFETIME(AShooterPS, NbKill);
}

void AShooterPS::CopyProperties(class APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
	
	if (PlayerState)
	{
		AShooterPS* ShooterPlayerState = Cast<AShooterPS>(PlayerState);
		if (ShooterPlayerState)
		{
			ShooterPlayerState->NbKill = NbKill;
			ShooterPlayerState->NbKill = NbDeath;
			ShooterPlayerState->ClientSetup = ClientSetup;
		}
	}
}

void AShooterPS::OverrideWith(class APlayerState* PlayerState)
{
	Super::OverrideWith(PlayerState);
	
	if (PlayerState)
	{
		const AShooterPS* ShooterPlayerState = Cast<AShooterPS>(PlayerState);
		if (ShooterPlayerState)
		{
			NbKill = ShooterPlayerState->NbKill;
			NbDeath = ShooterPlayerState->NbDeath;
		}
	}
}

void AShooterPS::Rep_ClientDatas()
{
	Super::Rep_ClientDatas();

	AShooterCharacter* Pawn = GetPawn<AShooterCharacter>();
	if (Pawn && Pawn->IsLocallyControlled())
	{
		UPlayerGI* GameInstance = GetWorld()->GetGameInstance<UPlayerGI>();
		GameInstance->SetUserInfo(ClientSetup);

		Pawn->SetTeam(ClientSetup.Team);
	}
}

void AShooterPS::Reset()
{
	NbKill = 0;
	NbDeath = 0;
}
