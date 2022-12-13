// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ShooterMultiPS_Base.generated.h"

UENUM(BlueprintType)
enum class ETeam : uint8
{
	None = 0,
	Blue,
	Red,
	AI,
};

USTRUCT(BlueprintType)
struct FClientSetup
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Name = "";

	UPROPERTY(BlueprintReadWrite)
	ETeam Team = ETeam::None;
	
	UPROPERTY(BlueprintReadWrite)
	bool bIsReady = false;
};
/**
 * 
 */
UCLASS()
class SHOOTERMULTI_API AShooterMultiPS_Base : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(ReplicatedUsing = Rep_ClientDatas, BlueprintReadWrite)
		FClientSetup ClientSetup;
	
protected:
	void BeginPlay() override;

public:	
	// Used to copy properties from the current PlayerState to the passed one
	virtual void CopyProperties(class APlayerState* PlayerState);
	// Used to override the current PlayerState with the properties of the passed one
	virtual void OverrideWith(class APlayerState* PlayerState);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SR_SetClientSetup(const FClientSetup& InSetup);
	
	UFUNCTION()
	virtual void Rep_ClientDatas();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
