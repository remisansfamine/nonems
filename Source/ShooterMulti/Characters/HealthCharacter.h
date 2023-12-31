#pragma once

#include "GameFramework/Character.h"
#include "../GameFramework/ShooterPS.h"
#include "../GameFramework/Resetable.h"
#include "ShooterMulti/Components/CompensatorLabel.h"
#include "HealthCharacter.generated.h"

UCLASS()
class SHOOTERMULTI_API AHealthCharacter : public ACharacter, public IResetable
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	UCompensatorLabel* LabelComponent = nullptr;

	float DisappearTimer;
	bool bIsDisappearing;
	TArray<UMaterialInstanceDynamic*> DissolveMaterials;

	UPROPERTY(ReplicatedUsing = Rep_PawnTeam, BlueprintReadOnly, Category = "Character")
	ETeam Team;

	UPROPERTY(EditAnywhere, Category = "Character|Health", meta = (ClampMin = "0.0"))
	float MaxHealth = 100.f;

	UPROPERTY(Replicated)
	float Health = MaxHealth;

	UPROPERTY(EditAnywhere, Category = "Character|Health", meta = (ClampMin = "0.0"))
	float DisapearingDelay = 10.f;

	UPROPERTY(EditAnywhere, Category = "Character|Health", meta = (ClampMin = "0.0"))
	float DisapearingDuration = 3.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Health")
	USoundBase* HitSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Health")
	USoundBase* HeadshotHitSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Health")
	USoundBase* PunchHitSound;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Character")
	class USphereComponent* PunchCollision;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character", meta = (ClampMin = "0"))
	float PunchDuration = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character", meta = (ClampMin = "0"))
	float PunchDamage = 10.f;

	void InitRagdoll();
	void ActivateRagdoll();

public:

	DECLARE_EVENT(AHealthCharacter, TeamSwitchEvent)
	TeamSwitchEvent OnTeamSwitch;

	UFUNCTION()
	void Rep_PawnTeam();
	
	AHealthCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintPure, Category = "Character|Health")
	bool IsDead() const;

	UFUNCTION(NetMulticast, Reliable)
	void Multi_OnDeath();

	UFUNCTION(BlueprintCallable, Category = "Character|Health")
	float GetMaxHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Character|Health")
	float GetHealth() const;

	ETeam GetTeam() const;

	void SetTeam(ETeam InTeam);

	UFUNCTION(BlueprintCallable, Category = "Character|Health")
	virtual float	TakeDamage	(float					DamageAmount,
								 FDamageEvent const&	DamageEvent,
								 class AController*		EventInstigator,
								 AActor*				DamageCauser) override;

	UFUNCTION(NetMulticast, Reliable)
	void Multi_OnTakeDamage(bool IsHeadshot, bool IsPunch, const FVector& HitLocation);
	
	UFUNCTION(BlueprintCallable, Category = "Character|Health")
	float GainHealth(float Amount);
	UFUNCTION(BlueprintCallable, Category = "Character|Health")
	void ResetHealth();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Character|Health")
	void SR_InflictPunch(float TimeStamp);

	void UpdateSkinColor();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Reset() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void StartDisappear();
	virtual void UpdateDisappear();
	virtual void FinishDisappear();
};
