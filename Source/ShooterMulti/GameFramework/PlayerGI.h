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
	int MaxGameScore = 1500; //Only on server
	
	UPROPERTY()
	int MaxGameTime = 600; //Only on server
	
public:
	UPROPERTY(BlueprintReadOnly)
	FString GameLevel = "";

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

	UFUNCTION()
	void SetGameMap(const FString& Name);

	UFUNCTION(BlueprintCallable)
	int GetMaxTime() const;
};