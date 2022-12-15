#pragma once

#include "ShooterMultiPS_Base.h"
#include "Engine/GameInstance.h"
#include "PlayerGI.generated.h"


UCLASS()
class SHOOTERMULTI_API UPlayerGI : public UGameInstance
{
	GENERATED_BODY()
	
	FClientSetup UserInfo;

	UPROPERTY()
	int MaxGameScore = 50; //Only on server
	
	UPROPERTY()
	int MaxGameTime = 300; //Only on server
	
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
	
	UFUNCTION()
	void SetMaxScore(const int Score);

	UFUNCTION(BlueprintCallable)
	int GetMaxScore() const;
	
	UFUNCTION()
	void SetMaxTime(const int Time);

	UFUNCTION(BlueprintCallable)
	int GetMaxTime() const;
};