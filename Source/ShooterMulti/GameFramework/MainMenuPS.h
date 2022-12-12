// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShooterMultiPS_Base.h"
#include "MainMenuPS.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERMULTI_API AMainMenuPS : public AShooterMultiPS_Base
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void SetNameInstance();
};
