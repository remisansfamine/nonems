#include "DeathMatchGS.h"
#include "ShooterPS.h"
#include "TimerManager.h"
#include "DeathMatchGM.h"
#include "../Characters/ShooterCharacter.h"
#include "../LD/Pickup.h"
#include "../Controllers/ShooterController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"

void ADeathMatchGS::BeginPlay()
{
	Super::BeginPlay();

	OnTeamWin.AddLambda([this](ETeam Team) { ShowTeamWinHUD(Team); });

	OnGameRestart.AddLambda([this]() { Reset(); });

	GameMode = Cast<ADeathMatchGM>(AuthorityGameMode);
	if (!GameMode)
		return;

	GameInstance = GetGameInstance<UPlayerGI>();
	CurrentTime = GameInstance->GetMaxTime();
	MaxTeamScore = GameInstance->GetMaxScore();
	
	GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ADeathMatchGS::AdvanceTimer, 1.0f, true);

	SetTeamsPlayerStarts();
}

void ADeathMatchGS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADeathMatchGS, RedTeamScore);
	DOREPLIFETIME(ADeathMatchGS, BlueTeamScore);
	DOREPLIFETIME(ADeathMatchGS, MaxTeamScore);
	DOREPLIFETIME(ADeathMatchGS, CurrentTime);
}

void ADeathMatchGS::SetTeamsPlayerStarts()
{
	if (BlueTeamPlayerStarts.Num() != 0 && RedTeamPlayerStarts.Num() != 0)
		return;
	
	// Find all player starts world actors
	TArray<AActor*> PlayerStartsActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStartsActors);

	// Setup teams player start list
	for (AActor* PStartActor : PlayerStartsActors)
	{
		APlayerStart* PStart = Cast<APlayerStart>(PStartActor);

		if (PStart->PlayerStartTag == "BlueTeam_Start")
			BlueTeamPlayerStarts.Add(PStart);
		else if (PStart->PlayerStartTag == "RedTeam_Start")
			RedTeamPlayerStarts.Add(PStart);
	}
}

AActor* ADeathMatchGS::FindValidPlayerStart(AController* Controller, const TArray<APlayerStart*>& PlayerStartList)
{
	TArray<APlayerStart*> UnOccupiedStartPoints;
	TArray<APlayerStart*> OccupiedStartPoints;
	
	UWorld* World = GetWorld();
	
	for (APlayerStart* PStart : PlayerStartList)
	{
		FVector ActorLocation = PStart->GetActorLocation();
		const FRotator ActorRotation = PStart->GetActorRotation();

		// Check if the pawn is fitting the spawn location
		// Else, check if its possible to spawn around the initial location
		if (!World->EncroachingBlockingGeometry(nullptr, ActorLocation, ActorRotation))
			UnOccupiedStartPoints.Add(PStart);
		else if (World->FindTeleportSpot(nullptr, ActorLocation, ActorRotation))
			OccupiedStartPoints.Add(PStart);
	}

	// Find random start
	if (UnOccupiedStartPoints.Num() > 0)
		return UnOccupiedStartPoints[FMath::RandRange(0, UnOccupiedStartPoints.Num() - 1)];
	if (OccupiedStartPoints.Num() > 0)
		return OccupiedStartPoints[FMath::RandRange(0, OccupiedStartPoints.Num() - 1)];
	
	return nullptr;
}

void ADeathMatchGS::AdvanceTimer()
{
	--CurrentTime;
	
	if (CurrentTime <= 0)
	{
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
		if (RedTeamScore < BlueTeamScore)
			UpdateEndHud(ETeam::Blue);
		else if (RedTeamScore > BlueTeamScore)
			UpdateEndHud(ETeam::Red);
		else
			UpdateEndHud(ETeam::None);
	}
}

void ADeathMatchGS::SetScore(ETeam Team, int32 NewScore)
{
	if (!GameMode)
		return;

	MaxTeamScore = GameMode->MaxKill;
	
	switch (Team)
	{
	case ETeam::Red:
		RedTeamScore += NewScore;
		
		if (RedTeamScore >= MaxTeamScore)
			UpdateEndHud(Team);

		break;

	case ETeam::Blue:
		BlueTeamScore += NewScore;
		if (BlueTeamScore >= MaxTeamScore)
			UpdateEndHud(Team);
		
		break;

	default:
		break;
	}
}
int32 ADeathMatchGS::GetScore(ETeam Team) const
{
	switch (Team)
	{
	case ETeam::Red:
		return RedTeamScore;

	case ETeam::Blue:
		return BlueTeamScore;

	default:
		return 0;
	}
}

void ADeathMatchGS::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	OnPlayerNum.Broadcast(this);
}

void ADeathMatchGS::RemovePlayerState(APlayerState* PlayerState)
{
	OnPlayerNum.Broadcast(this);

	Super::RemovePlayerState(PlayerState);
}

bool ADeathMatchGS::CanAddAI()
{
	return GameMode && GameMode->MaxAIPerPlayer* PlayerArray.Num() > CurrentAICount;
}

void ADeathMatchGS::AddAI()
{
	CurrentAICount++;
}

void ADeathMatchGS::RemoveAI()
{
	CurrentAICount--;
}

int ADeathMatchGS::GetNbplayer()
{
	return PlayerArray.Num();
}

void ADeathMatchGS::Multi_SetEndHud_Implementation(ETeam Team)
{
	OnTeamWin.Broadcast(Team);
}

void ADeathMatchGS::UpdateEndHud(ETeam Team)
{
	GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
	Multi_SetEndHud(Team);
}

void ADeathMatchGS::Reset()
{
	TArray<AActor*> Resetables;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UResetable::StaticClass(), Resetables);

	for (auto& res : Resetables)
		Cast<IResetable>(res)->Reset();
}

void ADeathMatchGS::Rep_TeamScores()
{
	if (OnScoresUpdated.IsBound())
		OnScoresUpdated.Broadcast();
}

void ADeathMatchGS::ResetAfterDelay()
{
	if (!GameMode)
		return;
	
	CurrentTime = GameMode->GameTime;
	GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ADeathMatchGS::AdvanceTimer, 1.0f, true);

	RedTeamScore = 0;
	BlueTeamScore = 0;
	CurrentAICount = 0;

	OnResetAfterDelay.Broadcast();
}

AActor* ADeathMatchGS::GetValidPlayerStart(AController* Controller)
{
	SetTeamsPlayerStarts();
	
	const AShooterPS* ShooterPState = Cast<AShooterPS>(Controller->PlayerState);
	if (ShooterPState->ClientSetup.Team == ETeam::Blue)
	{
		return FindValidPlayerStart(Controller, BlueTeamPlayerStarts);
	}
	if (ShooterPState->ClientSetup.Team == ETeam::Red)
	{
		return FindValidPlayerStart(Controller, RedTeamPlayerStarts);
	}
	
	return nullptr;
}

void ADeathMatchGS::EndGameTrigg()
{
	OnGameRestart.Broadcast();
}
