#include "ShooterCharacter.h"

//#include <Actor.h>
//#include <Audioclient.h>

#include "../Animations/ShooterCharacterAnim.h"
#include "../GameFramework/PlayerGI.h"
#include "../LD/EnemySpawnerButton.h"
#include "../Movements/ShooterCharacterMovement.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UObjectGlobals.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

AShooterCharacter::AShooterCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UShooterCharacterMovement>(CharacterMovementComponentName))
{
	DisapearingDelay = 1.5f;

	ShooterCharacterMovement = Cast<UShooterCharacterMovement>(GetCharacterMovement());

	// Create Weapon
	Weapon = CreateDefaultSubobject<UWeaponComponent>("Rifle");

	ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshContainer(TEXT("SkeletalMesh'/Game/Weapons/Rifle.Rifle'"));
	if (MeshContainer.Succeeded())
		Weapon->SetSkeletalMesh(MeshContainer.Object);

	Weapon->SetRelativeLocation(FVector(1.0f, 4.0f, -2.0f));
	Weapon->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	Weapon->SetupAttachment(GetMesh(), "hand_r");

	// Create Camera
	Camera = CreateDefaultSubobject<UPlayerCameraComponent>("PlayerCamera");
	Camera->SetupAttachment(RootComponent);

	PrimaryActorTick.bCanEverTick = true;
}

void AShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AShooterCharacter, State);
	DOREPLIFETIME(AShooterCharacter, AimPitch);
	DOREPLIFETIME(AShooterCharacter, AimYaw);
}

EShooterCharacterState AShooterCharacter::GetState() const
{
	return State;
}

void AShooterCharacter::SetState(EShooterCharacterState InState)
{
	State = InState;
}

UWeaponComponent* AShooterCharacter::GetWeaponComponent()
{
	return Weapon;
}

UPlayerCameraComponent* AShooterCharacter::GetCameraComponent()
{
	return Camera;
}

void AShooterCharacter::InitPlayer()
{
	AShooterPS* PS = GetPlayerState<AShooterPS>();
	
	SetTeam(PS->ClientSetup.Team);
}

void AShooterCharacter::Invincibility(float Duration)
{
	Health = 100000;
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, [this]() { Health = MaxHealth; }, Duration, false);

	InvincibilityFX(Duration);
}

void AShooterCharacter::BeginPlay()
{
	OnTeamSwitch.AddLambda([this]() { RefreshTeamHUD(Team); });
	
	Super::BeginPlay();

	if (ADeathMatchGM* GM = Cast<ADeathMatchGM>(GetWorld()->GetAuthGameMode()))
		Invincibility(GM->InvincibilityTime);
}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDead())
		return;

	if (bIsShooting && !Weapon->TryToShoot())
		StartWantsToReload();

	SR_UpdateAimOffsets();

	Camera->ShakeCamera(uint8(State), GetLastMovementInputVector().Size());
}

void AShooterCharacter::StartWantsToSprint()
{
	ShooterCharacterMovement->Safe_bWantsToSprint = true;
}

void AShooterCharacter::EndWantsToSprint()
{
	ShooterCharacterMovement->Safe_bWantsToSprint = false;
}

void AShooterCharacter::StartJump()
{
	if (bIsShooting)
		EndShoot();

	if (State == EShooterCharacterState::Aim)
		EndWantsToAim();
	else if (State == EShooterCharacterState::Reload)
		AbortReload();

	if (CanJump() && (State == EShooterCharacterState::IdleRun || State == EShooterCharacterState::Sprint))
	{
		SetState(EShooterCharacterState::Jump);
		Jump();
	}
}

void AShooterCharacter::EndJump()
{
	if (State != EShooterCharacterState::Jump && State != EShooterCharacterState::Falling)
		return;

	SetState(EShooterCharacterState::IdleRun);
	StopJumping();
}

void AShooterCharacter::StartWantsToAim()
{
	if (State != EShooterCharacterState::IdleRun)
		return;

	ShooterCharacterMovement->Safe_bWantsToAim = true;
}

void AShooterCharacter::EndWantsToAim()
{
	if (State != EShooterCharacterState::Aim)
		return;

	ShooterCharacterMovement->Safe_bWantsToAim = false;
}

void AShooterCharacter::StartShoot()
{
	if (State == EShooterCharacterState::IdleRun || State == EShooterCharacterState::Aim)
		bIsShooting = true;
}

void AShooterCharacter::EndShoot()
{
	bIsShooting = false;
}

void AShooterCharacter::StartWantsToReload()
{
	ShooterCharacterMovement->Safe_bWantsToReload = true;
}

void AShooterCharacter::EndWantsToReload()
{
	ShooterCharacterMovement->Safe_bWantsToReload = false;
}

void AShooterCharacter::AbortReload()
{
	if (State != EShooterCharacterState::Reload)
		return;

	SetState(EShooterCharacterState::IdleRun);

	ShooterCharacterMovement->StopReloading();
}

void AShooterCharacter::Falling()
{
	Super::Falling();

	if (State == EShooterCharacterState::Jump)
		return;

	if (bIsShooting)
		EndShoot();

	if (State == EShooterCharacterState::Aim)
		EndWantsToAim();
	else if (State == EShooterCharacterState::Reload)
		AbortReload();

	SetState(EShooterCharacterState::Falling);
}

void AShooterCharacter::CL_PushButton()
{
	PlayPushButtonAnim();

	SR_PushButton();
}

void AShooterCharacter::Multi_PushButton_Implementation()
{
	if (!IsLocallyControlled())
		PlayPushButtonAnim();
}

void AShooterCharacter::SR_PushButton_Implementation()
{
	if (bIsShooting)
		bIsShooting = false;
	else if (State == EShooterCharacterState::Reload)
		AbortReload();

	if (State != EShooterCharacterState::IdleRun)
		return;

	SetState(EShooterCharacterState::PushButton);
	
	Multi_PushButton();
}

void AShooterCharacter::SR_InflictPushButton()
{
	if (!HasAuthority())
		return;
	
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, TSubclassOf<AEnemySpawnerButton>());

	if (OverlappingActors.Num() > 0)
	{
		if (AEnemySpawnerButton* Button = Cast<AEnemySpawnerButton>(OverlappingActors[0]))
			Button->SR_Activate(Team);
	}
}

void AShooterCharacter::SR_UpdateAimOffsets()
{
	if (!HasAuthority())
		return;
	
	// Anim aim offsets
	FRotator LookRotation = UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation());
	AimPitch = UKismetMathLibrary::ClampAngle(LookRotation.Pitch, -90.f, 90.f);
	AimYaw = UKismetMathLibrary::ClampAngle(LookRotation.Yaw, -90.f, 90.f);
}

void AShooterCharacter::PlayPushButtonAnim()
{
	Cast<UShooterCharacterAnim>(GetMesh()->GetAnimInstance())->PlayPushButtonMontage();
}

void AShooterCharacter::CL_Punch()
{
	PlayPunchAnim();

	SR_Punch();
}

void AShooterCharacter::Multi_Punch_Implementation()
{
	if (!IsLocallyControlled())
		PlayPunchAnim();
}

void AShooterCharacter::SR_Punch_Implementation()
{
	if (bIsShooting)
		bIsShooting = false;
	else if (State == EShooterCharacterState::Reload)
		AbortReload();

	if (State != EShooterCharacterState::IdleRun)
		return;

	SetState(EShooterCharacterState::Punch);

	Multi_Punch();
}

void AShooterCharacter::PlayPunchAnim()
{
	Cast<UShooterCharacterAnim>(GetMesh()->GetAnimInstance())->PlayPunchMontage();
}

void AShooterCharacter::StartDisappear()
{
	Super::StartDisappear();
	
	FTimerHandle Handle1;
	GetWorld()->GetTimerManager().SetTimer(Handle1, [this]() { Weapon->SetVisibility(false, true); }, 3.5f, false);

	if (Controller)
	{
		APlayerController* PlayerControler = Cast<APlayerController>(Controller);
		PlayerControler->DisableInput(PlayerControler);
		
		FTimerHandle Handle2;
		GetWorld()->GetTimerManager().SetTimer(Handle2, [PlayerControler]() { PlayerControler->EnableInput(PlayerControler); }, 5.0f, false);
	}
}

void AShooterCharacter::FinishDisappear()
{
	APlayerController* PlayerController = Cast<APlayerController>(Controller);

	Super::FinishDisappear();

	if (ADeathMatchGM* GM = Cast<ADeathMatchGM>(GetWorld()->GetAuthGameMode()))
		GM->Respawn(PlayerController);
}

void AShooterCharacter::OnRep_IsAiming()
{
	if (!ShooterCharacterMovement)
		return;

	if (State == EShooterCharacterState::Aim)
		ShooterCharacterMovement->StartAiming(true);
	else
		ShooterCharacterMovement->StopAiming(true);
	
	ShooterCharacterMovement->bNetworkUpdateReceived = true;
}

void AShooterCharacter::OnRep_IsSprinting()
{
	if (!ShooterCharacterMovement)
		return;

	if (State == EShooterCharacterState::Sprint)
		ShooterCharacterMovement->StartSprinting(true);
	else
		ShooterCharacterMovement->StopSprinting(true);
	
	ShooterCharacterMovement->bNetworkUpdateReceived = true;
}

void AShooterCharacter::OnRep_IsReloading()
{
	if (!ShooterCharacterMovement)
		return;

	if (State == EShooterCharacterState::Reload)
		ShooterCharacterMovement->StartReloading(true);
	else
		ShooterCharacterMovement->StopReloading(true);
	
	ShooterCharacterMovement->bNetworkUpdateReceived = true;
}

void AShooterCharacter::OnStartAim()
{
	SetState(EShooterCharacterState::Aim);
	
	Camera->SwitchToAimCamera();
}

void AShooterCharacter::OnEndAim()
{
	Camera->SwitchToWalkCamera();
}

void AShooterCharacter::OnStartSprint()
{
	if (bIsShooting)
		EndShoot();

	if (State == EShooterCharacterState::Reload)
		AbortReload();
	else if (State == EShooterCharacterState::Aim)
		EndWantsToAim();

	if (State != EShooterCharacterState::IdleRun && State != EShooterCharacterState::Jump)
		return;

	SetState(EShooterCharacterState::Sprint);
}

void AShooterCharacter::OnEndSprint()
{
	
}

void AShooterCharacter::OnStartReload()
{
	if (Weapon && Weapon->AmmoCount > 0 && Weapon->WeaponMagazineSize > Weapon->LoadedAmmo)
	{
		if (State == EShooterCharacterState::Aim)
			EndWantsToAim();
		else if (bIsShooting)
			bIsShooting = false;

		if (State != EShooterCharacterState::IdleRun)
			return;

		SetState(EShooterCharacterState::Reload);
	}
}

void AShooterCharacter::OnEndReload()
{
	if (State != EShooterCharacterState::Reload)
		return;

	SetState(EShooterCharacterState::IdleRun);
}

void AShooterCharacter::ReloadWeapon()
{
	if (Weapon)
		Weapon->Reload();
}

