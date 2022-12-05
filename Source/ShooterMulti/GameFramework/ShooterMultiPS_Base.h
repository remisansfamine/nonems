// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayInEditorDataTypes.h"
#include "GameFramework/PlayerState.h"
#include "ShooterMultiPS_Base.generated.h"

USTRUCT(BlueprintType)
struct FSessionParams
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	FString Name = "";
	UPROPERTY(BlueprintReadWrite)
	int Slots = 10;
	UPROPERTY(BlueprintReadWrite)
	int MaxScore = 100;
};

/**
 * 
 */
UCLASS()
class SHOOTERMULTI_API AShooterMultiPS_Base : public APlayerState
{
	GENERATED_BODY()

protected:
	void BeginPlay() override;

public:	
	UPROPERTY(BlueprintReadWrite)
	FSessionParams SessionParams;
	
	// Used to copy properties from the current PlayerState to the passed one
	virtual void CopyProperties(class APlayerState* PlayerState);
	// Used to override the current PlayerState with the properties of the passed one
	virtual void OverrideWith(class APlayerState* PlayerState);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void Client_GetSessionParams();

	UFUNCTION(Server, Reliable)
	void Server_SetSessionParams(const FSessionParams& newSessionParams);
};
