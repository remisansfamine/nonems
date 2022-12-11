// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGS.h"

#include "UtilsFunctionsLibrary.h"

bool AMainMenuGS::LoadClientGameSetup(FClientSetup& ClientSetup, const FString& ClientIdentifier)
{
	return UUtilsFunctionsLibrary::LoadDataStructureFromIniFile(ClientSetup, FPaths::ProjectDir(), ClientIdentifier);
}

bool AMainMenuGS::SaveClientGameSetup(const FClientSetup& ClientSetup, const FString& ClientIdentifier)
{
	const TSharedPtr<FJsonObject> jsonObjectInput = FJsonObjectConverter::UStructToJsonObject(ClientSetup);
	return UUtilsFunctionsLibrary::SaveDataStructureToIniFile(ClientSetup, FPaths::ProjectDir(), ClientIdentifier);
}