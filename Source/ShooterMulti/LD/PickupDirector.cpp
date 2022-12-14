#include "PickupDirector.h"
#include "Engine/World.h"
#include "../GameFramework/DeathMatchGS.h"

APickupDirector::APickupDirector()
{
	// Only update if solo play or server
	bNetLoadOnClient = false;
	PrimaryActorTick.bCanEverTick = HasAuthority() ? true : false;
}

void APickupDirector::BeginPlay()
{
	Super::BeginPlay();

	//ADeathMatchGS* GameState = Cast<ADeathMatchGS>(GetWorld()->GetGameState());
	//GameState->OnPlayerNum.AddLambda([this](ADeathMatchGS* GS) { UpdateFrequencies(GS); }); // ??
}

void APickupDirector::RegisterPickup(const UPickupComponent& PickupComponent)
{
	IsSpawnFullArray.Add(false);
	SpawnPoints.Add(PickupComponent.GetOwner());
	bIsFull = false;
}

void APickupDirector::TrySpawnPickup()
{
	if (PickupBPs.Num() == 0 || bIsFull)
		return;
	
	const int MaxPoints = SpawnPoints.Num() - 1;
	int	RandomPoint = FMath::RandRange(0, MaxPoints);
	const int PrevPoint = RandomPoint;

	while (IsSpawnFullArray[RandomPoint])
	{
		RandomPoint = (RandomPoint + 1) % SpawnPoints.Num();
		if (RandomPoint == PrevPoint)
		{
			bIsFull = true;
			return;
		}
	}

	CurrentPickupIndex = (CurrentPickupIndex + 1) % PickupBPs.Num();
	if (SpawnPickup(CurrentPickupIndex, RandomPoint))
		IsSpawnFullArray[RandomPoint] = true;
}

bool APickupDirector::SpawnPickup(int pickupIndex, int spawnPointIndex)
{
	const auto pickupBP = PickupBPs[pickupIndex];
	const auto pickupLocation = SpawnPoints[spawnPointIndex]->GetActorLocation();
	const auto pickupRotation = SpawnPoints[spawnPointIndex]->GetActorRotation();

	const auto Pickup = GetWorld()->SpawnActor<APickup>(pickupBP, pickupLocation, pickupRotation);

	if (!Pickup)
		return false;
	
	Pickup->SpawnKey.ClassKey = pickupIndex;
	Pickup->SpawnKey.SpawnPointKey = spawnPointIndex;
	Pickup->Director = this;

	return true;
}

void APickupDirector::FreePickup(FSpawnKey Key)
{
	IsSpawnFullArray[Key.SpawnPointKey] = false;
	bIsFull = false;
}

void APickupDirector::SetFull(bool isFull)
{
	bIsFull = isFull;
}

void APickupDirector::Reset()
{
	bIsFull = false;

	for (int i = 0; i < IsSpawnFullArray.Num(); i++)
		IsSpawnFullArray[i] = false;
}

void APickupDirector::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	timer += DeltaSeconds;

	if (timer >= SecondPerSpawn)
	{
		timer = 0.f;
		TrySpawnPickup();
	}
}
