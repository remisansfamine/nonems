#include "DeathMatchGM.h"

#include "PlayerGI.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ShooterMulti/Characters/HealthCharacter.h"
#include "ShooterMulti/Controllers/ShooterController.h"

void ADeathMatchGM::BeginPlay()
{
	Super::BeginPlay();

	MaxKill = GetGameInstance<UPlayerGI>()->GetMaxScore();
	GameTime = GetGameInstance<UPlayerGI>()->GetMaxTime();
}

void ADeathMatchGM::Respawn(APlayerController* PlayerController)
{
	RestartPlayerAtPlayerStart(PlayerController, ChoosePlayerStart(PlayerController));
}

void ADeathMatchGM::Quit()
{
	FGenericPlatformMisc::RequestExit(false);
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}

void ADeathMatchGM::UpdateScoreOnDeath(AHealthCharacter* Killed, AHealthCharacter* Killer) const
{
	// If not an AI nor a Player, leave now
	if (!Killed || !Killer)
		return;
	
	AShooterPS* KilledPS = nullptr, *KillerPS = nullptr;
	
	{
		const AShooterController* KilledController = Cast<AShooterController>(Killed->GetController()), *KillerController = Cast<AShooterController>(Killer->GetController());

		if (KilledController)
			KilledPS = KilledController->GetPlayerState<AShooterPS>();

		if (KillerController)
			KillerPS = KillerController->GetPlayerState<AShooterPS>();
	}

	const bool IsKilledPlayer = KilledPS != nullptr, IsKillerPlayer = KillerPS != nullptr;
	
	if (IsKilledPlayer)
		KilledPS->NbDeath++;

	const ETeam KillerTeam = Killer->GetTeam();

	// Update Score on GameState only if Killer is from a team (No Spec nor AI team)
	if (KillerTeam != ETeam::Blue && KillerTeam != ETeam::Red)
		return;
	
	ADeathMatchGS* GS = GetWorld()->GetGameState<ADeathMatchGS>();

	const ETeam KilledTeam = Killed->GetTeam();

	int32 KillScore = 0;
	// Check if team kill another team
	if (KillerTeam == KilledTeam)
	{
		if (IsKillerPlayer)
			// When a Player team-kills an ally Player/AI
			KillScore = IsKilledPlayer ? ScoreFromAllyPlayerKill : ScoreFromAllyAIDeath;
	}
	else
	{
		if (IsKillerPlayer) // When a Player kills ...
		{
			if (IsKilledPlayer) // an enemy Player
			{
				KillerPS->NbKill++;
				// When a Player kills an enemy Player
				KillScore = ScoreFromEnemyPlayerKill;
			}
			else // an enemy AI
				// When a Player kills an enemy AI
				KillScore = ScoreFromEnemyAIDeath;
		}
		else if (IsKilledPlayer) // When an ally AI kills ...
			// When an ally AI kills an enemy Player
			KillScore = ScoreFromAllyAIKill;
	}

	GS->SetScore(KillerTeam, KillScore);
	
	if (IsKillerPlayer)
		KillerPS->SetScore(KillerPS->GetScore() + KillScore);
}

AActor* ADeathMatchGM::ChoosePlayerStart_Implementation(AController* Player)
{
	ADeathMatchGS* CurGameState = GetGameState<ADeathMatchGS>();

	return CurGameState->GetValidPlayerStart(Player);
}
