#include "ShooterCharacter.h"
#include "../Animations/ShooterCharacterAnim.h"
#include "../GameFramework/PlayerGI.h"
#include "../LD/EnemySpawnerButton.h"
#include "../Movements/ShooterCharacterMovement.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UObjectGlobals.h"
#include "Animation/AnimBlueprint.h"
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
	DOREPLIFETIME(AShooterCharacter, bIsAiming);
	DOREPLIFETIME(AShooterCharacter, bIsSprinting);
}

EShooterCharacterState AShooterCharacter::GetState() const
{
	return State;
}

void AShooterCharacter::SetState(EShooterCharacterState InState)
{
	PrevState = State;
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

void AShooterCharacter::UpdateAimOffsets(float Pitch, float Yaw)
{
	AimPitch = Pitch;
	AimYaw = Yaw;
}

void AShooterCharacter::InitPlayer()
{
	const FPlayerInfo& PlayerInfo = static_cast<UPlayerGI*>(GetGameInstance())->GetUserInfo();

	InitTeamColor(static_cast<ETeam>(PlayerInfo.TeamNum));
}

void AShooterCharacter::InitTeamColor(ETeam InTeam)
{
	SetTeam(InTeam);
	OnTeamSwitch.Broadcast();
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

	if (bIsShooting && !Weapon->Shot())
		StartReload();

	// Anim aim offsets
	FRotator LookRotation = UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation());
	AimPitch = UKismetMathLibrary::ClampAngle(LookRotation.Pitch, -90.f, 90.f);
	AimYaw = UKismetMathLibrary::ClampAngle(LookRotation.Yaw, -90.f, 90.f);

	UpdateAimOffsets(AimPitch, AimYaw);

	Camera->ShakeCamera(uint8(State), GetLastMovementInputVector().Size());
}

void AShooterCharacter::StartSprint()
{
	if (bIsShooting)
		EndShoot();

	if (State == EShooterCharacterState::Reload)
		AbortReload();
	else if (State == EShooterCharacterState::Aim)
		EndAim();

	if (State != EShooterCharacterState::IdleRun && State != EShooterCharacterState::Jump)
		return;

	if (State == EShooterCharacterState::Jump)
		PrevState = EShooterCharacterState::Sprint;
	else
		SetState(EShooterCharacterState::Sprint);

	ShooterCharacterMovement->StartSprinting();
}

void AShooterCharacter::EndSprint()
{
	if (State != EShooterCharacterState::Sprint && State != EShooterCharacterState::Jump)
		return;

	if (State == EShooterCharacterState::Jump)
		PrevState = EShooterCharacterState::IdleRun;
	else
		SetState(EShooterCharacterState::IdleRun);

	ShooterCharacterMovement->StopSprinting();
}

void AShooterCharacter::StartJump()
{
	if (bIsShooting)
		EndShoot();

	if (State == EShooterCharacterState::Aim)
		EndAim();
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

void AShooterCharacter::StartAim()
{
	if (State != EShooterCharacterState::IdleRun)
		return;

	ShooterCharacterMovement->Safe_bWantsToAim = true;
}

void AShooterCharacter::EndAim()
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

void AShooterCharacter::StartReload()
{
	if (Weapon && Weapon->AmmoCount > 0 && Weapon->WeaponMagazineSize > Weapon->LoadedAmmo)
	{
		if (State == EShooterCharacterState::Aim)
			EndAim();
		else if (bIsShooting)
			bIsShooting = false;

		if (State != EShooterCharacterState::IdleRun)
			return;

		SetState(EShooterCharacterState::Reload);

		ShooterCharacterMovement->StartReloading();
	}
}

void AShooterCharacter::EndReload()
{
	if (State != EShooterCharacterState::Reload)
		return;

	SetState(EShooterCharacterState::IdleRun);

	ShooterCharacterMovement->StopReloading();

	if(Weapon)
		Weapon->Reload();
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
		EndAim();
	else if (State == EShooterCharacterState::Reload)
		AbortReload();

	SetState(EShooterCharacterState::Falling);
}

void AShooterCharacter::PushButton()
{
	if (bIsShooting)
		bIsShooting = false;
	else if (State == EShooterCharacterState::Reload)
		AbortReload();

	if (State != EShooterCharacterState::IdleRun)
		return;

	SetState(EShooterCharacterState::PushButton);
	PlayPushButtonAnim();
}

void AShooterCharacter::InflictPushButton()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, TSubclassOf<AEnemySpawnerButton>());

	if (OverlappingActors.Num() > 0)
	{
		AEnemySpawnerButton* Button = Cast<AEnemySpawnerButton>(OverlappingActors[0]);
		
		if (Button)
			Button->Activate(Team);
	}
}

void AShooterCharacter::PlayPushButtonAnim()
{
	Cast<UShooterCharacterAnim>(GetMesh()->GetAnimInstance())->PlayPushButtonMontage();
}

void AShooterCharacter::Punch()
{
	if (bIsShooting)
		bIsShooting = false;
	else if (State == EShooterCharacterState::Reload)
		AbortReload();

	if (State != EShooterCharacterState::IdleRun)
		return;

	SetState(EShooterCharacterState::Punch);
	PlayPunchAnim();
}

void AShooterCharacter::PlayPunchAnim()
{
	Cast<UShooterCharacterAnim>(GetMesh()->GetAnimInstance())->PlayPunchMontage();
}

void AShooterCharacter::StartDisapear()
{
	Super::StartDisapear();
	
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

void AShooterCharacter::FinishDisapear()
{
	APlayerController* PlayerController = Cast<APlayerController>(Controller);

	Super::FinishDisapear();

	if (ADeathMatchGM* GM = Cast<ADeathMatchGM>(GetWorld()->GetAuthGameMode()))
		GM->Respawn(PlayerController);
}

void AShooterCharacter::OnRep_IsAiming()
{
	if (!ShooterCharacterMovement)
		return;
	
	if (bIsAiming)
	{
		ShooterCharacterMovement->Safe_bWantsToAim = true;
		ShooterCharacterMovement->StartAiming(true);
	}
	else
	{
		ShooterCharacterMovement->Safe_bWantsToAim = false;
		ShooterCharacterMovement->StopAiming(true);
	}
	
	ShooterCharacterMovement->bNetworkUpdateReceived = true;
}

void AShooterCharacter::OnRep_IsSprinting()
{
	
}

void AShooterCharacter::OnEndAim()
{
	SetState(PrevState);

	Camera->SwitchToWalkCamera();
}

void AShooterCharacter::OnStartAim()
{
	SetState(EShooterCharacterState::Aim);
	
	Camera->SwitchToAimCamera();
}
