#include "HealthCharacter.h"
#include "ShooterCharacter.h"
#include "../Weapons/DamageTypePunch.h"
#include "../GameFramework/DeathMatchGS.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine.h"
#include "Net/UnrealNetwork.h"

// COMMENTARY SECOND


AHealthCharacter::AHealthCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	LabelComponent = CreateDefaultSubobject<UCompensatorLabel>(TEXT("CompensatorLabel"));
	
	//Create Punch Collision
	PunchCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PunchCollision"));
	PunchCollision->SetupAttachment(RootComponent);
	PunchCollision->SetRelativeLocation(FVector(80.f, 0.f, 20.f));
	PunchCollision->InitSphereRadius(25.f);

	const ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshContainer(TEXT("SkeletalMesh'/Game/Resources/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (MeshContainer.Succeeded())
		GetMesh()->SetSkeletalMesh(MeshContainer.Object);

	Team = ETeam::None;
	
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	PrimaryActorTick.bCanEverTick = true;
}

void AHealthCharacter::BeginPlay()
{
	Super::BeginPlay();

	bIsDisappearing = false;
	ResetHealth();

	InitRagdoll();

	OnTeamSwitch.AddLambda([this]() { UpdateSkinColor(); });
	OnTeamSwitch.Broadcast(); // First refresh.
}

void AHealthCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsDisappearing)
		return;

	DisappearTimer += DeltaTime;

	UpdateDisappear();
}

void AHealthCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AHealthCharacter, Health);
	DOREPLIFETIME(AHealthCharacter, Team);
}

bool AHealthCharacter::IsDead() const
{
	return Health <= 0.0f;
}

void AHealthCharacter::Multi_OnDeath_Implementation()
{
	ActivateRagdoll();
	StartDisappear();
}

float AHealthCharacter::GetMaxHealth() const
{
	return MaxHealth;
}
float AHealthCharacter::GetHealth() const
{
	return Health;
}

ETeam AHealthCharacter::GetTeam() const
{
	return Team;
}

void AHealthCharacter::UpdateSkinColor()
{
	if (Team == ETeam::None)
		GetMesh()->SetVectorParameterValueOnMaterials("TeamColor", FVector(0, 1, 0)); // Green == bad init
	else if(Team == ETeam::AI)
		GetMesh()->SetVectorParameterValueOnMaterials("TeamColor", FVector(0.24, 0.24, 0.24)); // Black
	else
		GetMesh()->SetVectorParameterValueOnMaterials("TeamColor", FVector((Team != ETeam::Blue), 0, (Team != ETeam::Red)));
}

float AHealthCharacter::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser)
{
	if (!HasAuthority())
		return 0.f;
	
	AHealthCharacter* DamagingCharacter = Cast<AHealthCharacter>(DamageCauser);
	
	if (!IsValid(DamagingCharacter) || IsDead())
		return 0.0f;
	
	float TotalDamage = 0.f;

	const FPointDamageEvent* PointDamageEvent = nullptr;
	
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
		PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);

	if (!IsDead())
	{
		bool IsHeadshot = false;
		bool IsPunch = true;

		if (PointDamageEvent && PointDamageEvent->HitInfo.PhysMaterial.Get())
		{
			IsPunch = false;
			TotalDamage = DamageAmount * PointDamageEvent->HitInfo.PhysMaterial->DestructibleDamageThresholdScale;

			IsHeadshot = PointDamageEvent->HitInfo.PhysMaterial->DestructibleDamageThresholdScale > 1.0f;
		}
		else
			TotalDamage = DamageAmount;

		Health = FMath::Max(0.f, Health - TotalDamage);

		Multi_OnTakeDamage(IsHeadshot, IsPunch, PointDamageEvent->HitInfo.Location);
	}
	
	if (IsDead())
	{
		if (ADeathMatchGM* GameMode = GetWorld()->GetAuthGameMode<ADeathMatchGM>())
			GameMode->UpdateScoreOnDeath(this, DamagingCharacter);

		Multi_OnDeath();
	}

	return TotalDamage;
}

void AHealthCharacter::Multi_OnTakeDamage_Implementation(bool IsHeadshot, bool IsPunch, const FVector& HitLocation)
{
	USoundBase* CrtHitSound = PunchHitSound;

	if (!IsPunch)
	{
		CrtHitSound = IsHeadshot ? HeadshotHitSound : HitSound;
	}

	if (CrtHitSound)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), CrtHitSound, HitLocation);
}

float AHealthCharacter::GainHealth(float GainAmount)
{
	if(!IsDead() && GainAmount > 0.0f)
		Health = FMath::Min(Health + GainAmount, MaxHealth);

	return Health;
}

void AHealthCharacter::ResetHealth()
{
	Health = MaxHealth;
}

void AHealthCharacter::SR_InflictPunch_Implementation(float TimeStamp)
{
	FVector StartPos = GetActorLocation();
	FVector EndPos = PunchCollision->GetComponentLocation();

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.bTraceComplex = true;

	TArray<FHitResult> OutHits;

	ALagCompensator* Compensator = GetWorld()->GetGameState<ADeathMatchGS>()->GetLagCompensator();

	if (!Compensator)
		return;

	Compensator->SR_StartCompensation(TimeStamp);

	GetWorld()->SweepMultiByObjectType(
		OutHits, StartPos, EndPos, FQuat::Identity, 0,
		FCollisionShape::MakeSphere(PunchCollision->GetUnscaledSphereRadius()),
		Params);
	
	Compensator->SR_FinishCompensation();

	TArray<AHealthCharacter*> HitActors;

	for (FHitResult& Hit : OutHits)
	{
		AHealthCharacter* Character = Cast<AHealthCharacter>(Hit.Actor.Get());

		if (Character && !HitActors.Contains(Character))
		{
			FPointDamageEvent DamageEvent = FPointDamageEvent(PunchDamage, Hit, GetActorForwardVector(), UDamageTypePunch::StaticClass());
			Character->TakeDamage(PunchDamage, DamageEvent, nullptr, this);
			HitActors.Add(Character);
		}
	}
}

void AHealthCharacter::InitRagdoll()
{
	TArray<UActorComponent*> SkeletalComponents;
	GetComponents(USkeletalMeshComponent::StaticClass(), SkeletalComponents);
	for (UActorComponent* Component : SkeletalComponents)
	{
		USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(Component);
		TArray<UMaterialInterface*> Materials = SkeletalMeshComponent->GetMaterials();

		for (int i = 0; i < Materials.Num(); ++i)
		{
			UMaterialInstanceDynamic* NewMaterial = UMaterialInstanceDynamic::Create(Materials[i], this);
			SkeletalMeshComponent->SetMaterial(i, NewMaterial);
			DissolveMaterials.Add(NewMaterial);
		}
	}

	TArray<UActorComponent*> StaticMeshComponents;
	GetComponents(UStaticMeshComponent::StaticClass(), StaticMeshComponents);

	for (UActorComponent* Component : StaticMeshComponents)
	{
		UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(Component);
		TArray<UMaterialInterface*> Materials = StaticMeshComponent->GetMaterials();

		for (int i = 0; i < Materials.Num(); ++i)
		{
			UMaterialInstanceDynamic* NewMaterial = UMaterialInstanceDynamic::Create(Materials[i]->GetMaterial(), this);
			StaticMeshComponent->SetMaterial(i, NewMaterial);
			DissolveMaterials.Add(NewMaterial);
		}
	}

	for (UMaterialInstanceDynamic* Material : DissolveMaterials)
	{
		Material->SetScalarParameterValue(FName(TEXT("DissolveAmmount")), 0.f);
	}
}

void AHealthCharacter::ActivateRagdoll()
{
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	if (GetMesh() && Capsule)
	{
		TArray<UActorComponent*> SkeletalMeshComponents;
		GetComponents(USkeletalMeshComponent::StaticClass(), SkeletalMeshComponents);

		if (SkeletalMeshComponents.Num() > 0)
		{
			Capsule->SetCollisionResponseToAllChannels(ECR_Ignore);
			Capsule->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
			Capsule->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
			Capsule->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);

			for (UActorComponent* ActorComponent : SkeletalMeshComponents)
			{
				if (USkeletalMeshComponent * SkeletalMesh = Cast<USkeletalMeshComponent>(ActorComponent))
				{
					SkeletalMesh->bPauseAnims = true;

					SkeletalMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
					SkeletalMesh->SetCollisionObjectType(ECC_GameTraceChannel1);
					SkeletalMesh->SetCollisionResponseToAllChannels(ECR_Block);
					SkeletalMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
					SkeletalMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
					SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					SkeletalMesh->SetSimulatePhysics(true);
				}
			}
		}
	}
}

void AHealthCharacter::StartDisappear()
{
	DisappearTimer = 0.f;
	bIsDisappearing = true;
}

void AHealthCharacter::UpdateDisappear()
{
	if (!bIsDisappearing || DisappearTimer < DisapearingDelay)
		return;

	GetMesh()->SetScalarParameterValueOnMaterials(FName(TEXT("DissolveAmmount")), (DisappearTimer - DisapearingDelay) / DisapearingDuration);

	if (DisappearTimer > DisapearingDelay + DisapearingDuration)
	{
		bIsDisappearing = false;
		return FinishDisappear();
	}
}

void AHealthCharacter::FinishDisappear()
{
	Destroy();
}

void AHealthCharacter::Reset()
{
	StartDisappear();
}

void AHealthCharacter::SetTeam(ETeam InTeam)
{
	Team = InTeam;
}

void AHealthCharacter::Rep_PawnTeam()
{
	if (OnTeamSwitch.IsBound())
		OnTeamSwitch.Broadcast();
}