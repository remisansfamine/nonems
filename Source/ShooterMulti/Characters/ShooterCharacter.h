#pragma once

#include "HealthCharacter.h"
#include "../Weapons/WeaponComponent.h"
#include "PlayerCameraComponent.h"
#include "ShooterCharacter.generated.h"

UENUM(BlueprintType)
enum class EShooterCharacterState : uint8
{
	IdleRun,
	Aim,
	Sprint,
	Reload,
	Jump,
	Falling,
	Punch,
	Dead,
	PushButton
};

UCLASS()
class SHOOTERMULTI_API AShooterCharacter : public AHealthCharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Character|Shooter")
	UWeaponComponent* Weapon;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Character|Shooter")
	UPlayerCameraComponent* Camera;

	UPROPERTY(ReplicatedUsing=OnRep_StateChange, BlueprintReadOnly, Category = "Character|Shooter")
	EShooterCharacterState State;
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	float AimPitch;

	UPROPERTY(Replicated, BlueprintReadOnly)
	float AimYaw;

	void SR_UpdateAimOffsets();

	void PlayPushButtonAnim();

	void PlayPunchAnim();

	virtual void Falling() override;

	virtual void BeginPlay() override;

	void Invincibility(float Duration);

	UFUNCTION(BlueprintNativeEvent, Category = "Character|Shooter")
	void InvincibilityFX(float Duration);
	void InvincibilityFX_Implementation(float Duration) {};

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Movement)
	class UShooterCharacterMovement* ShooterCharacterMovement;
	
	UPROPERTY(BlueprintReadOnly, Replicated, Category=Character)
	uint32 bIsShooting:1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Shooter", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MinSprintMagnitude = .3f;

	AShooterCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	EShooterCharacterState GetState() const { return State; }
	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void SetState(EShooterCharacterState InState) { State = InState; }

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	UWeaponComponent* GetWeaponComponent() { return Weapon; }

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	UPlayerCameraComponent* GetCameraComponent() const { return Camera; }

	void InitPlayer();

	void InitTeamColor(ETeam InTeam);

	void Tick(float DeltaTime) override;

	UFUNCTION()
	void StartWantsToSprint();
	UFUNCTION()
	void EndWantsToSprint();

	UFUNCTION() void StartJump();
	UFUNCTION(Server, Reliable) void SR_StartJump();

	UFUNCTION() void EndJump();
	UFUNCTION(Server, Reliable) void SR_EndJump();


	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void StartWantsToAim();
	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void EndWantsToAim();

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void StartShoot();
	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void EndShoot();

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void StartWantsToReload();
	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void EndWantsToReload();
	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void AbortReload();

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")							void CL_PushButton();
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Character|Shooter")	void Multi_PushButton();
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Character|Shooter")			void SR_PushButton();
	
	
	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void SR_InflictPushButton();

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")							void CL_Punch();
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Character|Shooter")	void Multi_Punch();
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Character|Shooter")			void SR_Punch();

	UFUNCTION(BlueprintNativeEvent, Category = "Character|Shooter")
	void RefreshTeamHUD(ETeam InTeam);
	void RefreshTeamHUD_Implementation(ETeam InTeam) {};

	void StartDisappear() override;
	void FinishDisappear() override;
	
	/** Handle Aiming replicated from server */
	UFUNCTION() virtual void OnRep_StateChange(EShooterCharacterState PrevState);

	virtual void OnStartAim();
	virtual void OnEndAim();
	
	virtual void OnStartSprint();
    virtual void OnEndSprint();
    	
	virtual void OnStartReload();
	virtual void OnEndReload();

	void ReloadWeapon();
	bool CanReloadWeapon() const;
};