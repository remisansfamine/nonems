// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilsFunctionsLibrary.h"

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
