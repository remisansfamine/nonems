// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenuGM.h"

#include "UtilsFunctionsLibrary.h"

FString AMainMenuGM::GetServerDefaultMapName()
{
	FString ServerDefaultMap;
	GConfig->GetString(
	  TEXT("/Script/EngineSettings.GameMapsSettings"),
	  TEXT("ServerDefaultMap"),
	  ServerDefaultMap,
	  GEngineIni
	);

	TArray<FString> Out;
	ServerDefaultMap.ParseIntoArray(Out,TEXT("."),true);

	return Out.Last();
}

FString AMainMenuGM::FormatCommandParameters()
{
	const FString ProjectPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	const FString ProjectName = FString(FApp::GetProjectName());
	const FString DefaultMapName = AMainMenuGM::GetServerDefaultMapName();
	
	// Create command format arguments
	TArray<FStringFormatArg> args;
	args.Add(FStringFormatArg(ProjectPath));
	args.Add(FStringFormatArg(ProjectName));
	args.Add(FStringFormatArg(DefaultMapName));

	return FString::Format(TEXT(R"("{0}{1}.uproject" {2} -server -log -nostream)"), args);
}

void AMainMenuGM::LaunchServerInstance()
{
	// Create proc arguments
	const FString EngineExePath = FPaths::EngineDir() + "Binaries/Win64/UE4Editor.exe";
	const FString ServerLaunchParams = AMainMenuGM::FormatCommandParameters();

	// Create proc handle
	const FProcHandle WorkHandle = FPlatformProcess::CreateProc(*EngineExePath, *ServerLaunchParams,
		false, false, false, nullptr,
		0, nullptr, nullptr);

	// Check proc validity
	if (WorkHandle.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Launching Server!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to launch!"));
	}
}

bool AMainMenuGM::SaveServerConfig(const FServerConfig& config)
{
	const TSharedPtr<FJsonObject> jsonObjectInput = FJsonObjectConverter::UStructToJsonObject(config);
	return UUtilsFunctionsLibrary::SaveDataStructureToIniFile(config, FPaths::ProjectDir(), "config");
}
