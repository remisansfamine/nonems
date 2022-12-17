#include "ShooterCharacterMovement.h"
#include "../Characters/ShooterCharacter.h"

void UShooterCharacterMovement::FSavedMove_Shooter::Clear()
{
	Super::Clear();

	bWantsToSprint = bWantsToAim = bWantsToReload = 0;
}

void UShooterCharacterMovement::FSavedMove_Shooter::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	const UShooterCharacterMovement* ShooterMovement = Cast<UShooterCharacterMovement>(C->GetCharacterMovement());

	bWantsToSprint = ShooterMovement->Safe_bWantsToSprint;
	bWantsToAim = ShooterMovement->Safe_bWantsToAim;
	bWantsToReload = ShooterMovement->Safe_bWantsToReload;
}

bool UShooterCharacterMovement::FSavedMove_Shooter::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	const FSavedMove_Shooter* NewShooterMove = static_cast<FSavedMove_Shooter*>(NewMove.Get());

	if (bWantsToAim != NewShooterMove->bWantsToAim)
		return false;
	
	if (bWantsToSprint != NewShooterMove->bWantsToSprint)
		return false;

	if (bWantsToReload != NewShooterMove->bWantsToReload)
		return false;
	
	return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UShooterCharacterMovement::FSavedMove_Shooter::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	UShooterCharacterMovement* ShooterMovement = Cast<UShooterCharacterMovement>(C->GetCharacterMovement());
	
	ShooterMovement->Safe_bWantsToAim = bWantsToAim;
	ShooterMovement->Safe_bWantsToSprint = bWantsToSprint;
	ShooterMovement->Safe_bWantsToReload = bWantsToReload;
}

uint8 UShooterCharacterMovement::FSavedMove_Shooter::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();
	if (bWantsToSprint)
		Result |= FLAG_Custom_0;
	
	if (bWantsToAim)
		Result |= FLAG_Custom_1;

	if (bWantsToReload)
		Result |= FLAG_Custom_2;
		
	return Result;
}

UShooterCharacterMovement::FNetworkPredictionData_Client_Shooter::FNetworkPredictionData_Client_Shooter(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{
}

FSavedMovePtr UShooterCharacterMovement::FNetworkPredictionData_Client_Shooter::AllocateNewMove()
{
	return MakeShared<FSavedMove_Shooter>();
}

void UShooterCharacterMovement::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	Safe_bWantsToSprint = Flags & FSavedMove_Shooter::FLAG_Custom_0;
	Safe_bWantsToAim = Flags & FSavedMove_Shooter::FLAG_Custom_1;
	Safe_bWantsToReload = Flags & FSavedMove_Shooter::FLAG_Custom_2;
}

void UShooterCharacterMovement::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	if (MovementMode == MOVE_Walking)
	{
		if (Safe_bWantsToAim)
			MaxWalkSpeed = Aim_MaxWalkSpeed;
		
		else if (Safe_bWantsToReload)
			MaxWalkSpeed = Reload_MaxWalkSpeed;
		
		else if (Safe_bWantsToSprint)
			MaxWalkSpeed = Sprint_MaxWalkSpeed;
		
		else
			MaxWalkSpeed = Walk_MaxWalkSpeed;
	}
}

UShooterCharacterMovement::UShooterCharacterMovement()
	: Super()
{
	
}

void UShooterCharacterMovement::InitializeComponent()
{
	Super::InitializeComponent();
	
	ShooterCharacterOwner = Cast<AShooterCharacter>(PawnOwner);
}

FNetworkPredictionData_Client* UShooterCharacterMovement::GetPredictionData_Client() const
{
	if (ClientPredictionData)
		return ClientPredictionData;
	
	UShooterCharacterMovement* MutableThis = const_cast<UShooterCharacterMovement*>(this);
	
	MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Shooter(*this);
	MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
	MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;

	return ClientPredictionData;
}

void UShooterCharacterMovement::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	// Proxies get replicated aim state.
	if (!ShooterCharacterOwner || ShooterCharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)
		return;
	
	// Check for a change in aim state. Players toggle aim by changing bWantsToAim.
	const bool bIsAiming = ShooterCharacterOwner->GetState() == EShooterCharacterState::Aim;
	if (bIsAiming && (!Safe_bWantsToAim || !CanAimInCurrentState()))
	{
		StopAiming(false);
	}
	else if (!bIsAiming && Safe_bWantsToAim && CanAimInCurrentState())
	{
		StartAiming(false);
	}

	// Check for a change in sprint state. Players toggle sprint by changing bWantsToSprint.
	const bool bIsSprinting = ShooterCharacterOwner->GetState() == EShooterCharacterState::Sprint;
	if (bIsSprinting && (!Safe_bWantsToSprint || !CanSprintInCurrentState()))
	{
		StopSprinting(false);
	}
	else if (!bIsSprinting && Safe_bWantsToSprint && CanSprintInCurrentState())
	{
		StartSprinting(false);
	}

	// Check for a change in reload state. Players toggle reload by changing bWantsToReload.
	const bool bIsReloading = ShooterCharacterOwner->GetState() == EShooterCharacterState::Reload;
	if (bIsReloading && (!Safe_bWantsToReload || !CanReloadInCurrentState()))
	{
		StopReloading(false);
	}
	else if (!bIsReloading && Safe_bWantsToReload && CanReloadInCurrentState())
	{
		StartReloading(false);
	}
}

void UShooterCharacterMovement::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
	// Proxies get replicated aim state.
	if (!ShooterCharacterOwner || ShooterCharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)
		return;
	
	// Stop aim if no longer allowed to aim
	const bool bIsAiming = ShooterCharacterOwner->GetState() == EShooterCharacterState::Aim;
	if (bIsAiming && !CanAimInCurrentState())
		StopAiming(false);

	// Stop sprint if no longer allowed to sprint
	const bool bIsSprinting = ShooterCharacterOwner->GetState() == EShooterCharacterState::Sprint;
	if (bIsSprinting && !CanSprintInCurrentState())
		StopSprinting(false);

	// Stop reload if no longer allowed to reload
	const bool bIsReloading = ShooterCharacterOwner->GetState() == EShooterCharacterState::Reload;
	if (bIsReloading && !CanReloadInCurrentState())
		StopReloading(false);
}

void UShooterCharacterMovement::StartReloading(bool bClientSimulation)
{
	if (!HasValidData() || !bClientSimulation && !CanReloadInCurrentState())
		return;

	if (!bClientSimulation)
		ShooterCharacterOwner->SetState(EShooterCharacterState::Reload);
	
	ShooterCharacterOwner->OnStartReload();
}

void UShooterCharacterMovement::StopReloading(bool bClientSimulation)
{
	if (!bClientSimulation)
		ShooterCharacterOwner->SetState(EShooterCharacterState::IdleRun);
	
	ShooterCharacterOwner->OnEndReload();
}

void UShooterCharacterMovement::StartAiming(bool bClientSimulation)
{
	if (!HasValidData() || !bClientSimulation && !CanAimInCurrentState())
		return;

	if (!bClientSimulation)
		ShooterCharacterOwner->SetState(EShooterCharacterState::Aim);
	
	ShooterCharacterOwner->OnStartAim();
}

void UShooterCharacterMovement::StopAiming(bool bClientSimulation)
{
	if (!bClientSimulation)
		ShooterCharacterOwner->SetState(EShooterCharacterState::IdleRun);
	
	ShooterCharacterOwner->OnEndAim();
}

void UShooterCharacterMovement::StartSprinting(bool bClientSimulation)
{
	if (!HasValidData() || !bClientSimulation && !CanSprintInCurrentState())
		return;

	if (!bClientSimulation)
		ShooterCharacterOwner->SetState(EShooterCharacterState::Sprint);
	
	ShooterCharacterOwner->OnStartSprint();
}

void UShooterCharacterMovement::StopSprinting(bool bClientSimulation)
{
	if (!bClientSimulation)
		ShooterCharacterOwner->SetState(EShooterCharacterState::IdleRun);
	
	ShooterCharacterOwner->OnEndSprint();
}

bool UShooterCharacterMovement::CanAimInCurrentState() const
{
	return !IsFalling() && UpdatedComponent && !UpdatedComponent->IsSimulatingPhysics();
}

bool UShooterCharacterMovement::CanSprintInCurrentState() const
{
	return UpdatedComponent && !UpdatedComponent->IsSimulatingPhysics();
}

bool UShooterCharacterMovement::CanReloadInCurrentState() const
{
	return !IsFalling() && UpdatedComponent && !UpdatedComponent->IsSimulatingPhysics() && ShooterCharacterOwner->CanReloadWeapon();
}

