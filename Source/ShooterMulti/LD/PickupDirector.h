#pragma once

#include "Pickup.h"
#include "../GameFramework/Resetable.h"
#include "GameFramework/Actor.h"
#include "ShooterMulti/Components/PickupComponent.h"
#include "PickupDirector.generated.h"

UCLASS()
class SHOOTERMULTI_API APickupDirector : public AActor, public IResetable
{
	GENERATED_BODY()
	
private:

	TArray<bool> IsSpawnFullArray;
	int CurrentPickupIndex = 0;

	float timer = 0.f;

	//FTimerHandle TimerHandle;


protected:

	virtual void BeginPlay() override;

public:

	APickupDirector();

	bool bIsFull = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Director, meta = (ClampMin = 0.1f))
	float SecondPerSpawn = 15.0f;

	UPROPERTY()
	TArray<AActor*> SpawnPoints;

	UPROPERTY(EditInstanceOnly, BlueprintInternalUseOnly, Category = Director)
	TArray<TSubclassOf<APickup>> PickupBPs;

	void RegisterPickup(const UPickupComponent& PickupComponent);
	
	void TrySpawnPickup();
	void FreePickup(FSpawnKey Key);

	bool SpawnPickup(int pickupIndex, int spawnPointIndex);

	void SetFull(bool isFull);

	//void UpdateFrequencies(class ADeathMatchGS* GameState);

	virtual void Reset() override;

	virtual void Tick(float DeltaSeconds) override;
};