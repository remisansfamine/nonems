// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyRoomGM.h"

#include "HAL/PlatformFilemanager.h"
#include "Serialization/JsonSerializer.h"
#include "JsonUtilities/Public/JsonObjectConverter.h"

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

