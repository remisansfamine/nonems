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
		uint32 bWantsToAim:1;
		uint32 bWantsToReload:1;

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

	UPROPERTY()
	class AShooterCharacter* ShooterCharacterOwner = nullptr;

protected:
	UPROPERTY(EditDefaultsOnly) float Sprint_MaxWalkSpeed;
	UPROPERTY(EditDefaultsOnly) float Reload_MaxWalkSpeed;
	UPROPERTY(EditDefaultsOnly) float Aim_MaxWalkSpeed;
	UPROPERTY(EditDefaultsOnly) float Walk_MaxWalkSpeed;

	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	
public:
	bool Safe_bWantsToSprint = false;
	bool Safe_bWantsToReload = false;
	bool Safe_bWantsToAim = false;
	
	UShooterCharacterMovement();

	virtual void InitializeComponent() override;
	
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable) void StartAiming(bool bClientSimulation = false);
	UFUNCTION(BlueprintCallable) void StopAiming(bool bClientSimulation = false);
	UFUNCTION(BlueprintCallable) void StartReloading(bool bClientSimulation = false);
	UFUNCTION(BlueprintCallable) void StopReloading(bool bClientSimulation = false);
	UFUNCTION(BlueprintCallable) void StartSprinting(bool bClientSimulation = false);
	UFUNCTION(BlueprintCallable) void StopSprinting(bool bClientSimulation = false);

	virtual bool CanAimInCurrentState() const;
	virtual bool CanSprintInCurrentState() const;
	virtual bool CanReloadInCurrentState() const;
};
