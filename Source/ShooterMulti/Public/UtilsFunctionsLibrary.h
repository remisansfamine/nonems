// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UtilsFunctionsLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERMULTI_API UUtilsFunctionsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category = "Utils")
    static bool IsEOSEnabled();	
};
