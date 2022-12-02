// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGM.h"

void AMainMenuGM::LaunchServerInstance()
{
	FString ProjectPath = " /k ";
	ProjectPath.Append(FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()));
	//ProjectPath.Append("StartDedicatedServer.bat");
	ProjectPath.Append("StartServerThroughCMD.bat");
	UE_LOG(LogTemp, Warning, TEXT("%s"), *ProjectPath);

	FProcHandle WorkHandle = FPlatformProcess::CreateProc(TEXT("cmd"), *ProjectPath, false, false, false, nullptr, 0, nullptr, nullptr);

	if (WorkHandle.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Launching Server!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to launch!"));
	}
}
