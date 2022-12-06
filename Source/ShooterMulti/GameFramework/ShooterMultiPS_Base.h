// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayInEditorDataTypes.h"
#include "GameFramework/PlayerState.h"
#include "ShooterMultiPS_Base.generated.h"

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
	// Used to copy properties from the current PlayerState to the passed one
	virtual void CopyProperties(class APlayerState* PlayerState);
	// Used to override the current PlayerState with the properties of the passed one
	virtual void OverrideWith(class APlayerState* PlayerState);
};
