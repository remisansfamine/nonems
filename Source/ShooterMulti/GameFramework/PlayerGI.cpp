#include "PlayerGI.h"

UPlayerGI::UPlayerGI(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
}

FClientSetup UPlayerGI::GetUserInfo()
{
	return UserInfo;
}

void UPlayerGI::LeaveToMainMenu()
{
	Super::ReturnToMainMenu();
}

void UPlayerGI::SetUserInfo(const FClientSetup& NewSetup)
{
	UserInfo = NewSetup;
}

void UPlayerGI::SetUsername(const FString& InUserName)
{
	UserInfo.Name = InUserName;
}

const FString& UPlayerGI::GetUsername()
{
	return UserInfo.Name;
}

void UPlayerGI::SetMaxScore(const int Score)
{
	MaxGameScore = Score;
}

int UPlayerGI::GetMaxScore() const
{
	return MaxGameScore;
}
