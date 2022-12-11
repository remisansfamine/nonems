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

public:
	UPROPERTY(BlueprintReadOnly)
	int NbKill;
	UPROPERTY(BlueprintReadOnly)
	int NbDeath;

	// Used to copy properties from the current PlayerState to the passed one
	virtual void CopyProperties(class APlayerState* PlayerState);
	// Used to override the current PlayerState with the properties of the passed one
	virtual void OverrideWith(class APlayerState* PlayerState);

	UFUNCTION()
	void Reset();
};
