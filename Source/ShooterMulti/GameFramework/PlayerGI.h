#pragma once

#include "ShooterMultiPS_Base.h"
#include "Engine/GameInstance.h"
#include "PlayerGI.generated.h"


UCLASS()
class SHOOTERMULTI_API UPlayerGI : public UGameInstance
{
	GENERATED_BODY()
	
	FClientSetup UserInfo;

public:
	UPlayerGI(const FObjectInitializer& ObjInit);
	
	FClientSetup GetUserInfo();

	UFUNCTION(BlueprintCallable)
	void LeaveToMainMenu();
	
	UFUNCTION(BlueprintCallable)
	void SetUserInfo(const FClientSetup& NewSetup);
	
	UFUNCTION(BlueprintCallable)
	void SetUsername(const FString& InUserName);

	UFUNCTION(BlueprintCallable)
	const FString& GetUsername();
	
	UFUNCTION(BlueprintCallable)
	void SetTeamNum(const ETeam& InTeamNum);
};