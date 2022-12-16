
#pragma once

#include "GameFramework/Actor.h"
#include "../Characters/UndeadCharacter.h"
#include "../GameFramework/Resetable.h"
#include "EnemySpawnerButton.generated.h"

UCLASS()
class SHOOTERMULTI_API AEnemySpawnerButton : public AActor, public IResetable
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(ReplicatedUsing = OnRep_TeamChange)
	ETeam mTeam = ETeam::None;
	
	FTimerHandle mSpawnTimerHandle;

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_TeamChange() const;
	
public:

	UPROPERTY(EditAnywhere, BlueprintInternalUseOnly)
	float SecondPerSpawn = 3.0f;

	UPROPERTY(EditAnywhere)
	USoundBase* ActivateSound;

	UPROPERTY()
	UMaterialInstanceDynamic* material;

	AEnemySpawnerButton();

	void SR_Activate(ETeam team);

	UFUNCTION(Client, Reliable)
	void CL_OnActivate();

	UFUNCTION()
	void Reset() override;
};
