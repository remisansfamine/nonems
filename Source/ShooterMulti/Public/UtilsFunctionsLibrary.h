// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Private/DataTableJSON.h"
#include "JsonUtilities/Public/JsonObjectConverter.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UtilsFunctionsLibrary.generated.h"

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

	// Take a string and remove all the characters given in the WrongCharacters string
	UFUNCTION(BlueprintCallable, Category = "Utils")
	static FString StringCleanFromInput(const FString& EntryString, const FString& WrongCharacters, bool CaseSensitive);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static FString StringToAlnumString(const FString& EntryString, bool ShouldRemoveSpaces);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static bool IsAlpha(const FString& str);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static bool IsDigit(const FString& str);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static bool IsAlnum(const FString& str);

	static bool IsAlpha(const char c);
	static bool IsDigit(const char c);
	static bool IsAlnum(const char c);

	template<typename InStructType>
	static bool SaveDataStructureToIniFile(const InStructType& InStruct, const FString& FilePath, const FString& FileName);

	template<typename OutStructType>
	static bool LoadDataStructureFromIniFile(OutStructType& OutStruct, const FString& FilePath, const FString& FileName);
};

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

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Start loading data");
	
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*ProjectPath))
		return false;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Try load file");

	FString fileOutput;
	if (!FFileHelper::LoadFileToString(fileOutput, *ProjectPath))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Fail to load file");
		return false;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Try deserialize");
	
	TSharedPtr<FJsonObject> jsonObjectOutput;
	if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(fileOutput), jsonObjectOutput))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Fail deserialize");
		return false;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Try build object");
	
	if (!FJsonObjectConverter::JsonObjectToUStruct<OutStructType>(jsonObjectOutput.ToSharedRef(), &OutStruct))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Fail build object");
		return false;	
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Success");
	
	return true;
}
