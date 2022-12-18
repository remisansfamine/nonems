// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyRoomGM.h"

#include "LobbyRoomGS.h"
#include "PlayerGI.h"
#include "UtilsFunctionsLibrary.h"
#include "Kismet/GameplayStatics.h"

AActor* ALobbyRoomGM::ChoosePlayerStart_Implementation(AController* Player)
{
	return nullptr;
}

void ALobbyRoomGM::BeginPlay()
{
	Super::BeginPlay();

	// Fill the map names array
	for (auto& MapPair : MapPreviewsDico)
		MapNames.Add(MapPair.Key);

	ALobbyRoomGS* LobbyGS = GetGameState<ALobbyRoomGS>();

	// Update replicated preview name and texture
	const FString MapName = MapNames[LobbyGS->MapIndex];
	LobbyGS->CurrentMapPreviewName = MapName;
	LobbyGS->SetNewCurrentMap(MapName, MapPreviewsDico[MapName]->GetPathName());
	
	UPlayerGI* GameInstance = GetGameInstance<UPlayerGI>();
	if (GameInstance)
		GameInstance->SetGameMap(MapName);
}

void ALobbyRoomGM::OnMapSelectionChange(int Dir)
{
	ALobbyRoomGS* LobbyGS = GetGameState<ALobbyRoomGS>();
	
	// Update map index (replicate to give authority to the server)
	LobbyGS->MapIndex += (Dir == -1) ? (MapNames.Num() - 1) : 1;
	LobbyGS->MapIndex %= MapNames.Num();

	// Update replicated preview name and texture
	const FString MapName = MapNames[LobbyGS->MapIndex];
	LobbyGS->CurrentMapPreviewName = MapName;
	LobbyGS->SetNewCurrentMap(MapName, MapPreviewsDico[MapName]->GetPathName());

	UPlayerGI* GameInstance = GetGameInstance<UPlayerGI>();
	if (GameInstance)
		GameInstance->SetGameMap(MapName);
}

bool ALobbyRoomGM::ParseServerConfig(FServerConfig& config)
{
	return UUtilsFunctionsLibrary::LoadDataStructureFromIniFile(config, FPaths::ProjectDir(), "config");
}

void ALobbyRoomGM::SetMaxScoreInInstance(const int Score)
{
	UPlayerGI* GameInstance = GetGameInstance<UPlayerGI>();

	if (GameInstance)
		GameInstance->SetMaxScore(Score);
}

void ALobbyRoomGM::SetMaxTimeInInstance(const int Time)
{
	UPlayerGI* GameInstance = GetGameInstance<UPlayerGI>();

	if (GameInstance)
		GameInstance->SetMaxTime(Time);
}
