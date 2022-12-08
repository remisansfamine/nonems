// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyRoomGM.h"

#include "HAL/PlatformFilemanager.h"
#include "Serialization/JsonSerializer.h"
#include "GameFrameWork/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "JsonUtilities/Public/JsonObjectConverter.h"

AActor* ALobbyRoomGM::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<AActor*> FoundPlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), FoundPlayerStarts);

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, "Set player start");
	
	for (auto& actor : FoundPlayerStarts)
	{
		APlayerStart* playerStart = Cast<APlayerStart>(actor);
		if (playerStart->PlayerStartTag != FName("Taken"))
		{
			playerStart->PlayerStartTag = FName("Taken");
			return playerStart;
		}
	}
	
	return nullptr;

}

bool ALobbyRoomGM::ParseServerConfig(FServerConfig& config)
{
	config = FServerConfig();
	
	FString ProjectPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	ProjectPath.Append(FString("config.ini"));
	
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*ProjectPath))
	{
		return false;
	}

	FString fileOutput;

	if (!FFileHelper::LoadFileToString(fileOutput, *ProjectPath))
	{
		return false;
	}

	TSharedPtr<FJsonObject> jsonObjectOutput;

	if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(fileOutput), jsonObjectOutput))
	{
		return false;
	}

	if (!FJsonObjectConverter::JsonObjectToUStruct<FServerConfig>(jsonObjectOutput.ToSharedRef(), &config))
	{
		return false;
	}
	
	return true;
}

