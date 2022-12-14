// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Private/DataTableJSON.h"
#include "JsonUtilities/Public/JsonObjectConverter.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UtilsFunctionsLibrary.generated.h"

#define PRINT(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5, FColor::White,text)

/**
 * 
 */
UCLASS()
class SHOOTERMULTI_API UUtilsFunctionsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category = "Utils")
    static bool IsEOSEnabled();

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static FString GetComputerName();

	// Take a string and remove all the characters given in the WrongCharacters string
	UFUNCTION(BlueprintCallable, Category = "Utils")
	static FString StringCleanFromInput(const FString& EntryString, const FString& WrongCharacters, const bool CaseSensitive);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static FString StringToAlnumString(const FString& EntryString, const bool ShouldRemoveSpaces);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static bool IsAlpha(const FString& Str);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static bool IsDigit(const FString& Str);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static bool IsAlnum(const FString& Str);

	static bool IsAlpha(const char C);
	static bool IsDigit(const char C);
	static bool IsAlnum(const char C);

	template<typename InStructType>
	static bool SaveDataStructureToIniFile(const InStructType& InStruct, const FString& FilePath, const FString& FileName);

	template<typename OutStructType>
	static bool LoadDataStructureFromIniFile(OutStructType& OutStruct, const FString& FilePath, const FString& FileName);
};

inline FString UUtilsFunctionsLibrary::GetComputerName()
{
	return FString(FWindowsPlatformProcess::ComputerName());
}

template <typename InStructType>
bool UUtilsFunctionsLibrary::SaveDataStructureToIniFile(const InStructType& InStruct, const FString& FilePath, const FString& FileName)
{
	const TSharedPtr<FJsonObject> jsonObjectInput = FJsonObjectConverter::UStructToJsonObject(InStruct);
	
	if (jsonObjectInput == nullptr)
		return false;
	
	FString fileInput;
		
	if (!FJsonSerializer::Serialize(jsonObjectInput.ToSharedRef(), TJsonWriterFactory<>::Create(&fileInput, 0)))
		return false;
		
	FString ProjectPath = FPaths::ConvertRelativePathToFull(FilePath);
	ProjectPath.Append(FString(FileName + ".ini"));
		
	if (!FFileHelper::SaveStringToFile(fileInput, *ProjectPath))
		return false;
	
	return true;
}

template <typename OutStructType>
bool UUtilsFunctionsLibrary::LoadDataStructureFromIniFile(OutStructType& OutStruct, const FString& FilePath, const FString& FileName)
{
	OutStruct = OutStructType();
	
	FString ProjectPath = FPaths::ConvertRelativePathToFull(FilePath);
	ProjectPath.Append(FString(FileName + ".ini"));
	
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*ProjectPath))
		return false;

	FString fileOutput;
	if (!FFileHelper::LoadFileToString(fileOutput, *ProjectPath))
		return false;
	
	TSharedPtr<FJsonObject> jsonObjectOutput;
	if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(fileOutput), jsonObjectOutput))
		return false;
	
	if (!FJsonObjectConverter::JsonObjectToUStruct<OutStructType>(jsonObjectOutput.ToSharedRef(), &OutStruct))
		return false;	
	
	return true;
}
