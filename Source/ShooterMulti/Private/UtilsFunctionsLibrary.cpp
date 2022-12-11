// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilsFunctionsLibrary.h"

#include <cctype>

#include "OnlineSubsystem.h"

bool UUtilsFunctionsLibrary::IsEOSEnabled()
{
	IOnlineSubsystem* OSS = IOnlineSubsystem::Get();

	if (OSS)
	{
		FString Name = OSS->GetOnlineServiceName().ToString();
		if (Name == FString("EOS"))
			return true;
	}
	
	return false;
}

FString UUtilsFunctionsLibrary::StringCleanFromInput(const FString& EntryString, const FString& WrongCharacters, bool CaseSensitive)
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

FString UUtilsFunctionsLibrary::StringToAlnumString(const FString& EntryString, bool ShouldRemoveSpaces)
{
	FString OutString;
	
	for (const char c : EntryString)
	{
		if (IsAlnum(c) || (!ShouldRemoveSpaces && c == ' '))
			OutString += c;
	}

	return OutString;
}

bool UUtilsFunctionsLibrary::IsAlpha(const FString& str)
{
	// Should be a character
	if (str.Len() != 1)
		return false;

	return IsAlpha(str[0]);
}

bool UUtilsFunctionsLibrary::IsDigit(const FString& str)
{
	// Should be a character
	if (str.Len() != 1)
		return false;

	return IsDigit(str[0]);
}

bool UUtilsFunctionsLibrary::IsAlnum(const FString& str)
{
	// Should be a character
	if (str.Len() != 1)
		return false;

	return IsAlnum(str[0]);
}

bool UUtilsFunctionsLibrary::IsAlpha(const char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool UUtilsFunctionsLibrary::IsDigit(const char c)
{
	return (c >= '0' && c <= '9');
}

bool UUtilsFunctionsLibrary::IsAlnum(const char c)
{
	return IsDigit(c) || IsAlpha(c);
}
