
#include "AmmoPickup.h"
#include "../Characters/ShooterCharacter.h"

void AAmmoPickup::NotifyActorBeginOverlap(AActor * OtherActor)
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No authority to pick ammo"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Pick ammo from server"));
	AShooterCharacter* Player = Cast<AShooterCharacter>(OtherActor);
	UWeaponComponent* Weapon = Player ? Player->GetWeaponComponent() : nullptr;

	if (!Weapon || Weapon->AmmoCount >= Weapon->MaxAmmo)
		return;
	
	Weapon->GetAmmo(Capacity);

	// After because of Destroy
	Super::NotifyActorBeginOverlap(OtherActor);
}