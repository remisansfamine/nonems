// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyRoomGM.h"

#include "PlayerGI.h"
#include "UtilsFunctionsLibrary.h"
#include "Kismet/GameplayStatics.h"

AActor* ALobbyRoomGM::ChoosePlayerStart_Implementation(AController* Player)
{
	UE_LOG(LogTemp, Warning, TEXT("Choose player start impl"));
	return nullptr;
	//TArray<AActor*> FoundPlayerStarts;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), FoundPlayerStarts);
//
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, "Set player start");
	//
	//for (auto& actor : FoundPlayerStarts)
	//{
	//	APlayerStart* playerStart = Cast<APlayerStart>(actor);
	//	if (playerStart->PlayerStartTag != FName("Taken"))
	//	{
	//		playerStart->PlayerStartTag = FName("Taken");
	//		return playerStart;
	//	}
	//}
	//
	//return nullptr;

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

