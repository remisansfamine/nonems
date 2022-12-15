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
	GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ADeathMatchGS::AdvanceTimer, 1.0f, true);
}

void ADeathMatchGS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADeathMatchGS, RedTeamScore);
	DOREPLIFETIME(ADeathMatchGS, BlueTeamScore);
	DOREPLIFETIME(ADeathMatchGS, CurrentTime);
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

	switch (Team)
	{
	case ETeam::Red:
		RedTeamScore += NewScore;
		
		if (RedTeamScore >= GameMode->MaxKill)
			UpdateEndHud(Team);

		break;

	case ETeam::Blue:
		BlueTeamScore += NewScore;
		if (BlueTeamScore >= GameMode->MaxKill)
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

void ADeathMatchGS::UpdateEndHud(ETeam Team)
{
	GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
	OnTeamWin.Broadcast(Team);
}

void ADeathMatchGS::Reset()
{
	TArray<AActor*> Resetables;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UResetable::StaticClass(), Resetables);

	for (auto& res : Resetables)
		Cast<IResetable>(res)->Reset();
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

void ADeathMatchGS::EndGameTrigg()
{
	OnGameRestart.Broadcast();
}
