
#pragma once

#include "HealthCharacter.h"
#include "UndeadCharacter.generated.h"

UENUM(BlueprintType)
enum class EUndeadCharacterState : uint8
{
	IdleRun,
	Punch,
	Stun,
	Dead
};

UCLASS()
class SHOOTERMULTI_API AUndeadCharacter : public AHealthCharacter
{
	GENERATED_BODY()

protected:

	//bool bHasPunched;
	UPROPERTY(BlueprintReadOnly, Category = "Character|Undead")
	EUndeadCharacterState State;
	EUndeadCharacterState PrevState;

	UFUNCTION(NetMulticast, Unreliable)
	void PlayPunchMontage();

	UFUNCTION(NetMulticast, Unreliable)
	void PlayHitMontage();

	void BeginPlay() override;

public:

	UPROPERTY(Category = UndeadCharacter, EditAnywhere, BlueprintReadWrite)
	float PunchCooldown = 1.5f;

	UPROPERTY(Category = UndeadCharacter, EditAnywhere, BlueprintReadWrite)
	float StunCooldown = 1.0f;

	AUndeadCharacter();

	void StartDisappear() override;
	void Reset() override;

	UFUNCTION(BlueprintCallable, Category = "Character|Undead")
	void StartStun();
	UFUNCTION()
	void EndStun();
	UFUNCTION(BlueprintCallable, Category = "Character|Undead")
	bool Punch();
	UFUNCTION()
	void EndPunch();
	UFUNCTION(BlueprintCallable, Category = "Character|Undead")
	EUndeadCharacterState GetState() const;
	UFUNCTION(BlueprintCallable, Category = "Character|Undead")
	void SetState(EUndeadCharacterState InState);

	virtual float	TakeDamage(float	DamageAmount,
										FDamageEvent const& DamageEvent,
										class AController* EventInstigator,
										AActor* DamageCauser) override;
};
