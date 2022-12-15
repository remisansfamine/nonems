#include "DeathMatchGM.h"

#include "PlayerGI.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"

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