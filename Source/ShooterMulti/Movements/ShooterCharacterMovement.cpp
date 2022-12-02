#include "ShooterCharacterMovement.h"
#include "../Characters/ShooterCharacter.h"

void UShooterCharacterMovement::FSavedMove_Shooter::Clear()
{
	Super::Clear();

	bWantsToSprint = 0;
}

void UShooterCharacterMovement::FSavedMove_Shooter::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	UShooterCharacterMovement* ShooterMovement = Cast<UShooterCharacterMovement>(C->GetCharacterMovement());

	bWantsToSprint = ShooterMovement->Safe_bWantsToSprint;
}

bool UShooterCharacterMovement::FSavedMove_Shooter::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	const FSavedMove_Shooter* NewShooterMove = static_cast<FSavedMove_Shooter*>(NewMove.Get());
	
	return bWantsToSprint != NewShooterMove->bWantsToSprint &&
		   Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UShooterCharacterMovement::FSavedMove_Shooter::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	UShooterCharacterMovement* ShooterMovement = Cast<UShooterCharacterMovement>(C->GetCharacterMovement());
	
	ShooterMovement->Safe_bWantsToSprint = bWantsToSprint;
}

uint8 UShooterCharacterMovement::FSavedMove_Shooter::GetCompressedFlags() const
{
	if (!bWantsToSprint)
		return Super::GetCompressedFlags();
	
	return Super::GetCompressedFlags() | FLAG_Custom_0;
}

UShooterCharacterMovement::FNetworkPredictionData_Client_Shooter::FNetworkPredictionData_Client_Shooter(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{
}

FSavedMovePtr UShooterCharacterMovement::FNetworkPredictionData_Client_Shooter::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_Shooter());
}

void UShooterCharacterMovement::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	Safe_bWantsToSprint = Flags & FSavedMove_Shooter::FLAG_Custom_0;
}

void UShooterCharacterMovement::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	if (MovementMode == MOVE_Walking)
		MaxWalkSpeed = Safe_bWantsToSprint ? Sprint_MaxWalkSpeed : Walk_MaxWalkSpeed;
}

UShooterCharacterMovement::UShooterCharacterMovement()
	: Super()
{
	
}

FNetworkPredictionData_Client* UShooterCharacterMovement::GetPredictionData_Client() const
{
	check(PawnOwner)

	if (ClientPredictionData)
		return ClientPredictionData;
	
	UShooterCharacterMovement* MutableThis = const_cast<UShooterCharacterMovement*>(this);
	
	MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Shooter(*this);
	MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
	MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;

	return ClientPredictionData;
}

void UShooterCharacterMovement::SprintPressed()
{
	Safe_bWantsToSprint = true;
}

void UShooterCharacterMovement::SprintReleased()
{
	Safe_bWantsToSprint = false;
}
