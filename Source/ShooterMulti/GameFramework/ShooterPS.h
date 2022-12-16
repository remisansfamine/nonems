#pragma once

#include "DeathMatchGM.h"
#include "PlayerGI.h"
#include "ShooterMultiPS_Base.h"
#include "ShooterPS.generated.h"

UCLASS()
class SHOOTERMULTI_API AShooterPS : public AShooterMultiPS_Base
{
	GENERATED_BODY()

protected:
	void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(Replicated, BlueprintReadOnly)
	int NbKill = 0;
	UPROPERTY(Replicated, BlueprintReadOnly)
	int NbDeath = 0;

	// Used to copy properties from the current PlayerState to the passed one
	virtual void CopyProperties(class APlayerState* PlayerState) override;
	// Used to override the current PlayerState with the properties of the passed one
	virtual void OverrideWith(class APlayerState* PlayerState) override;
	
	virtual void Rep_ClientDatas() override;
	
	UFUNCTION()
	void Reset();
};
