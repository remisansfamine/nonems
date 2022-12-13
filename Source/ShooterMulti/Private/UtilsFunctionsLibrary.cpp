// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilsFunctionsLibrary.h"

#include <cctype>

#include "OnlineSubsystem.h"

bool UUtilsFunctionsLibrary::IsEOSEnabled()
{
	const IOnlineSubsystem* OSS = IOnlineSubsystem::Get();

	if (OSS)
	{
		const FString Name = OSS->GetOnlineServiceName().ToString();
		if (Name == FString("EOS"))
			return true;
	}
	
	return false;
}

FString UUtilsFunctionsLibrary::StringCleanFromInput(const FString& EntryString, const FString& WrongCharacters, const bool CaseSensitive)
{
	FString OutString;

	for (const char EntryChar : EntryString)
	{
		bool IsWrong = false;
		
		for (const char WrongChar : WrongCharacters)
		{
			IsWrong |= (EntryChar == WrongChar);

			if (!CaseSensitive)
				IsWrong |= (std::tolower(EntryChar) == std::tolower(WrongChar));
		}

		if (!IsWrong)
			OutString += EntryChar;
	}

	return OutString;
}

FString UUtilsFunctionsLibrary::StringToAlnumString(const FString& EntryString, const bool ShouldRemoveSpaces)
{
	FString OutString;
	
	for (const char c : EntryString)
	{
		if (IsAlnum(c) || (!ShouldRemoveSpaces && c == ' '))
			OutString += c;
	}

	return OutString;
}

bool UUtilsFunctionsLibrary::IsAlpha(const FString& Str)
{
	// Should be a character
	if (Str.Len() != 1)
		return false;

	return IsAlpha(Str[0]);
}

bool UUtilsFunctionsLibrary::IsDigit(const FString& Str)
{
	// Should be a character
	if (Str.Len() != 1)
		return false;

	return IsDigit(Str[0]);
}

bool UUtilsFunctionsLibrary::IsAlnum(const FString& Str)
{
	// Should be a character
	if (Str.Len() != 1)
		return false;

	return IsAlnum(Str[0]);
}

bool UUtilsFunctionsLibrary::IsAlpha(const char C)
{
	return (C >= 'a' && C <= 'z') || (C >= 'A' && C <= 'Z');
}

bool UUtilsFunctionsLibrary::IsDigit(const char C)
{
	return (C >= '0' && C <= '9');
}

bool UUtilsFunctionsLibrary::IsAlnum(const char C)
{
	return IsDigit(C) || IsAlpha(C);
}
