#include "WeaponComponent.h"
#include "BeamLight.h"
#include "DamageTypeRifle.h"
#include "../Characters/ShooterCharacter.h"
#include "../Controllers/ShooterController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	LightPool.BeginPlay(GetWorld(), 4u);

	AmmoCount = MaxAmmo;
	CurrentSpread = 0.f;
	if (AmmoCount > WeaponMagazineSize)
	{
		AmmoCount -= WeaponMagazineSize - LoadedAmmo;
		LoadedAmmo = WeaponMagazineSize;
	}
	else
	{
		LoadedAmmo = AmmoCount;
		AmmoCount = 0;
	}
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UWeaponComponent, MaxAmmo);
	DOREPLIFETIME(UWeaponComponent, LoadedAmmo);
	DOREPLIFETIME(UWeaponComponent, AmmoCount);
}

void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	ShootTimer += DeltaTime;

	//update spread
	AShooterCharacter* Character = static_cast<AShooterCharacter*>(GetOwner());
	float MinSpread = (Character->GetState() == EShooterCharacterState::Aim) ? WeaponMinSpreadAim : WeaponMinSpreadWalk;
	CurrentSpread = FMath::Max(MinSpread, CurrentSpread - WeaponSpreadRecoveryRate * DeltaTime);
}

void UWeaponComponent::Shoot()
{
	FLaserWeaponData WeaponData;
	WeaponData.MuzzleTransform = GetSocketTransform("MuzzleFlashSocket");
	WeaponData.LookTransform = Cast<AShooterCharacter>(GetOwner())->GetCameraComponent()->GetCameraHandle()->GetComponentTransform();
	WeaponData.Damages = WeaponDamage;
	WeaponData.Knockback = WeaponKnockback;
	WeaponData.Spread = CurrentSpread;

	FHitResult HitResult;
	
	if (ShootLaser(GetOwner(), HitResult, WeaponData))
	{
		if (GetOwner()->HasAuthority())
			Multi_SetPointOfImpact(HitResult);
	}
	
	GetOwner()->HasAuthority() ? Multi_PlayShotFX(HitResult.ImpactPoint, WeaponData) : PlayShotFX(HitResult.ImpactPoint, WeaponData);
}

void UWeaponComponent::SR_TryToShoot_Implementation(float TimeStamp)
{
	if (ALagCompensator* Compensator = GetWorld()->GetGameState<ADeathMatchGS>()->GetLagCompensator())
	{
		Compensator->SR_StartCompensation(TimeStamp);
		TryToShoot();
		Compensator->SR_FinishCompensation();
	}
}

void UWeaponComponent::Multi_SetPointOfImpact_Implementation(const FHitResult& HitResult)
{
	//make impact decal
	MakeImpactDecal(HitResult, ImpactDecalMat, .9f * ImpactDecalSize, 1.1f * ImpactDecalSize);

	//create impact particles
	MakeImpactParticles(ImpactParticle, HitResult, .66f);
}

void UWeaponComponent::PlayShotFX(const FVector& ImpactPoint, const FLaserWeaponData& WeaponData)
{
	//make the beam visuals
	MakeLaserBeam(WeaponData.MuzzleTransform.GetLocation(), ImpactPoint, BeamParticle, BeamIntensity, FLinearColor(1.f, 0.857892f, 0.036923f), BeamIntensityCurve);
	
	//play the shot sound
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShotSound, WeaponData.MuzzleTransform.GetLocation());
	
	//make muzzle smoke
	UGameplayStatics::SpawnEmitterAttached(MuzzleSmokeParticle, this, FName("MuzzleFlashSocket"));
	
	//apply shake
	AShooterController* PlayerController = Cast<AShooterController>(Cast<AShooterCharacter>(GetOwner())->GetController());
	if (PlayerController && PlayerController->IsLocalPlayerController() && ShootShake)
		PlayerController->ClientStartCameraShake(ShootShake);
	
	//add spread
	CurrentSpread = FMath::Min(WeaponMaxSpread, CurrentSpread + WeaponSpreadPerShot);
	
	//play sound if gun empty
	if (LoadedAmmo == 0)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShotEmptySound, GetOwner()->GetActorLocation());
}


void UWeaponComponent::Multi_PlayShotFX_Implementation(const FVector& ImpactPoint, const FLaserWeaponData& WeaponData)
{
	if (GetOwner()->GetLocalRole() != ROLE_AutonomousProxy)
		PlayShotFX(ImpactPoint, WeaponData);
}

bool UWeaponComponent::TryToShoot()
{
	if (ShootTimer < FireRate)
		return true;
	
	ShootTimer = 0.f;

	if (LoadedAmmo <= 0)
		return false;

	--LoadedAmmo;

	if (!GetOwner()->HasAuthority())
		SR_TryToShoot(GetWorld()->GetGameState()->GetServerWorldTimeSeconds());

	Shoot();
	
	return true;
}

void UWeaponComponent::Reload()
{
	if (AmmoCount > WeaponMagazineSize)
	{
		AmmoCount -= WeaponMagazineSize - LoadedAmmo;
		LoadedAmmo = WeaponMagazineSize;
	}
	else
	{
		LoadedAmmo = AmmoCount;
		AmmoCount = 0;
	}
}

void UWeaponComponent::GetAmmo(int Count)
{
	AmmoCount = FMath::Min(AmmoCount + Count, MaxAmmo);
}

// Weapon Utiliy

bool UWeaponComponent::ShootLaser(AActor* Causer, FHitResult& HitResult, const FLaserWeaponData& WeaponData)
{
	FVector LookLocation = WeaponData.LookTransform.GetLocation();
	FVector LookDirection = WeaponData.LookTransform.GetRotation().GetForwardVector();

	//apply spread
	if (WeaponData.Spread > 0.f)
		LookDirection = UKismetMathLibrary::RandomUnitVectorInConeInRadians(LookDirection,
																			FMath::DegreesToRadians(WeaponData.Spread * .5f));

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Causer);
	CollisionParams.bTraceComplex = true;
	CollisionParams.bReturnPhysicalMaterial = true;

	bool DidHit = GetWorld()->LineTraceSingleByChannel(	HitResult, LookLocation, LookLocation + LookDirection * WeaponData.MaxDistance, ECC_Visibility , CollisionParams);
	
	//in case of actor hit
	if (!DidHit)
	{
		//when no actor hit
		HitResult.ImpactPoint = LookLocation + LookDirection * WeaponData.MaxDistance;
		HitResult.Distance = WeaponData.MaxDistance;
		return false;
	}
	
	//make damages
	FPointDamageEvent DamageEvent = FPointDamageEvent(WeaponData.Damages, HitResult, LookDirection, UDamageTypeRifle::StaticClass());
	HitResult.Actor->TakeDamage(WeaponData.Damages, DamageEvent, nullptr, Causer);

	//push hit actors (physics)
	TArray<UActorComponent*> SkeletalMeshComponents;
	HitResult.Actor->GetComponents(USkeletalMeshComponent::StaticClass(), SkeletalMeshComponents);
	for (auto ActorComponent : SkeletalMeshComponents)
	{
		USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(ActorComponent);
		if (SkeletalMeshComponent->IsSimulatingPhysics())
			SkeletalMeshComponent->AddForceAtLocation(LookDirection * WeaponData.Knockback, HitResult.ImpactPoint, HitResult.BoneName);
	}
	
	return Cast<ACharacter>(HitResult.Actor) == nullptr; // if collision with non character.
}

void UWeaponComponent::MakeImpactDecal(	const FHitResult& FromHit,
										UMaterialInterface* ImpactDecalMaterial,
										float ImpactDecalSizeMin,
										float ImpactDecalSizeMax)
{
	auto StaticMeshComponent = FromHit.Actor->FindComponentByClass<UStaticMeshComponent>();
	if (StaticMeshComponent)
	{
		FVector DecalPos = FromHit.ImpactPoint;
		FRotator DecalRot = (FromHit.ImpactNormal.Rotation().Quaternion() * FRotator(0.f, 0.f, FMath::RandRange(-180.f, 180.f)).Quaternion()).Rotator();
		float RandomSize = FMath::RandRange(ImpactDecalSizeMin, ImpactDecalSizeMax);
		FVector DecalSize = FVector(RandomSize, RandomSize, RandomSize);

		if (StaticMeshComponent->Mobility == EComponentMobility::Static)
		{
			UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), ImpactDecalMaterial, DecalSize,
				DecalPos, DecalRot, 0.f);
			if (DecalComponent)
				DecalComponent->FadeScreenSize = 11.f;
		}
		else
		{
			UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAttached(ImpactDecalMaterial, DecalSize, StaticMeshComponent,
				NAME_None, DecalPos, DecalRot, EAttachLocation::KeepWorldPosition, 0.f);
			if (DecalComponent)
				DecalComponent->FadeScreenSize = 11.f;
		}
	}
}

void UWeaponComponent::MakeLaserBeam(	FVector Start,
										FVector End,
										UParticleSystem* BeamParticles,
										float InBeamIntensity,
										FLinearColor Color,
										UCurveFloat* InBeamIntensityCurve)
{
	FTransform ParticleTransform;
	ParticleTransform.SetLocation(Start);
	ParticleTransform.SetRotation((End - Start).Rotation().Quaternion());

	//create a beam particle
	SpawnEmitterAtLocation(BeamParticles, ParticleTransform, Start, End);

	LightPool.Spawn(0.8f)->Initialize(Start, End, Color, 0.8f, InBeamIntensity, InBeamIntensityCurve);
}

void UWeaponComponent::MakeImpactParticles(UParticleSystem* ImpactParticles, const FHitResult& FromHit, float Scale)
{
	FTransform HitTransform;
	HitTransform.SetLocation(FromHit.ImpactPoint);
	HitTransform.SetRotation(FromHit.Normal.Rotation().Quaternion());
	HitTransform.SetScale3D(FVector(Scale, Scale, Scale));

	SpawnEmitterAtLocation(ImpactParticles, HitTransform);
}

void UWeaponComponent::SpawnEmitterAtLocation(	UParticleSystem* EmitterTemplate,
												const FTransform& SpawnTransform,
												const FVector& Source,
												const FVector& Target)
{
	if (!EmitterTemplate || IsRunningDedicatedServer())
		return;
	
	UParticleSystemComponent* ParticleSystemComponent = UGameplayStatics::SpawnEmitterAtLocation(	GetWorld(),
																				EmitterTemplate,
																				SpawnTransform,
																				true,
																				EPSCPoolMethod::AutoRelease);
	if (Source != FVector::ZeroVector && Target != FVector::ZeroVector)
	{
		ParticleSystemComponent->SetBeamSourcePoint(0, Source, 0);
		ParticleSystemComponent->SetBeamTargetPoint(0, Target, 0);
	}
}
