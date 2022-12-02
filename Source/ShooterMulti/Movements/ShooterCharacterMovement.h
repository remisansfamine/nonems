// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ShooterCharacterMovement.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERMULTI_API UShooterCharacterMovement : public UCharacterMovementComponent
{
	GENERATED_BODY()
private:
	class FSavedMove_Shooter : public FSavedMove_Character
	{
	public:
		using Super = FSavedMove_Character;
		
		uint32 bWantsToSprint:1;

		virtual void Clear() override;
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData) override;
		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		virtual void PrepMoveFor(ACharacter* C) override;
		virtual uint8 GetCompressedFlags() const override;

	};

	class FNetworkPredictionData_Client_Shooter : public FNetworkPredictionData_Client_Character
	{
	public:
		using Super = FNetworkPredictionData_Client_Character;
		
		FNetworkPredictionData_Client_Shooter(const UCharacterMovementComponent& ClientMovement);

		virtual FSavedMovePtr AllocateNewMove() override;
	};
	
protected:
	UPROPERTY(EditDefaultsOnly) float Sprint_MaxWalkSpeed;
	UPROPERTY(EditDefaultsOnly) float Walk_MaxWalkSpeed;

	bool Safe_bWantsToSprint = false;
	
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	
public:
	UShooterCharacterMovement();
	
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	UFUNCTION(BlueprintCallable)
	void SprintPressed();
	UFUNCTION(BlueprintCallable)
	void SprintReleased();
};
