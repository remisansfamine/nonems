#include "Pickup.h"
#include "PickupDirector.h"
#include "../Characters/ShooterCharacter.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APickup::BeginPlay()
{
	Super::BeginPlay();

	BasePos = GetActorLocation();
}

void APickup::Tick(float DeltaTime)
{
	GlobalTime += DeltaTime;
	float Offset = 10 * sin(GlobalTime);

	SetActorLocation(BasePos + FVector::UpVector * Offset);

	FRotator NewRot = GetActorRotation();
	NewRot.Yaw += 100 * DeltaTime;
	SetActorRotation(NewRot);
}

void APickup::NotifyActorBeginOverlap(AActor * OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	AShooterCharacter* Player = Cast<AShooterCharacter>(OtherActor);

	if (!IsValid(Player))
		return;

	Multi_PlaySoundLocally();

	if (IsValid(Director))
		Director->FreePickup(SpawnKey);
	
	Destroy();
}

void APickup::Multi_PlaySoundLocally_Implementation()
{
	if (IsRunningDedicatedServer())
		return;

	PlaySoundLocally();
}

void APickup::PlaySoundLocally()
{
	//play the shot sound
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, GetActorLocation());
}

void APickup::Reset()
{
	Destroy();
}
