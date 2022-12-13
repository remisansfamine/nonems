#include "UndeadDirector.h"
#include "../ShooterMulti.h"
#include "../GameFramework/DeathMatchGS.h"
#include "../Characters/UndeadCharacter.h"
#include "Engine/World.h"

AUndeadDirector* AUndeadDirector::Instance = nullptr;

AUndeadDirector::AUndeadDirector()
{
	bAlwaysRelevant = true;
}

// Called when the game starts or when spawned
void AUndeadDirector::BeginPlay()
{
	Super::BeginPlay();

	// Instance only used on Server
	Instance = this;

	if (SpawnPoints.Num() == 0)
		UE_LOG(GLogShooterMulti, Warning, TEXT("Undead Director has no spawn point."));

	if (IsActive && HasAuthority())
		GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &AUndeadDirector::SpawnTickEnemy, SecondPerSpawn, true);
}

void AUndeadDirector::Destroyed()
{
	GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);

	if (Instance == this)
		Instance = nullptr;
}

void AUndeadDirector::SpawnEnemy(FVector pos, const FRotator& rot, ETeam Team)
{
	ADeathMatchGS* temp = Cast<ADeathMatchGS>(GetWorld()->GetGameState());

	if (SpawnPoints.Num() == 0 || !temp->CanAddAI())
		return;

	pos.Y += 100; // avoid in ground spawn.

	temp->AddAI();

	FActorSpawnParameters spawnParameters;
	spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	auto undead = GetWorld()->SpawnActor<AUndeadCharacter>(UndeadBlueprint, pos, rot, spawnParameters);

	undead->SetTeam(Team);
}
void AUndeadDirector::SpawnTickEnemy()
{
	if (SpawnPoints.Num() == 0)
		return;
	
	const int rand = FMath::RandRange(0, SpawnPoints.Num() - 1);

	const AActor* SpawnPoint = SpawnPoints[rand];
	SpawnEnemy(SpawnPoint->GetActorLocation(), SpawnPoint->GetActorRotation());
}

AUndeadDirector* AUndeadDirector::GetInstance()
{
	return Instance;
}