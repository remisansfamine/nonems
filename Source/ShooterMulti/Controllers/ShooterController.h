#pragma once

#include "ShooterMultiController_Base.h"
#include "GameFramework/PlayerController.h"
#include "../GameFramework/DeathMatchGS.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ShooterController.generated.h"

UCLASS()
class SHOOTERMULTI_API AShooterController : public AShooterMultiController_Base
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	class AShooterCharacter* ShooterCharacter = nullptr;

	virtual void BeginPlayingState() override;
	virtual void SetupInputComponent() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void LookUp(float Value);
	void Turn(float Value);

	void StartSprint();
	void EndSprint();
	void StartJump();
	void StartAim();
	void EndAim();
	void StartShoot();
	void EndShoot();
	void StartReload();
	void PushButton();
	void Punch();

public:
	void DisableInput(APlayerController* PlayerController) override;

	UFUNCTION(BlueprintCallable, Category = "Shooter|PlayerController")
	void EndJump();
	UFUNCTION(BlueprintCallable, Category = "Shooter|PlayerController")
	void EndReload();
};