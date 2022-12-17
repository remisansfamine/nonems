
#include "ShooterCharacterAnim.h"
#include "AnimationUtilities.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PawnMovementComponent.h"
#include "ShooterMulti/Movements/ShooterCharacterMovement.h"

void UShooterCharacterAnim::NativeInitializeAnimation()
{
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

void UShooterCharacterAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!IsValid(ShooterCharacter))
		return;

	FVector2D const Movement = static_cast<FVector2D>(ShooterCharacter->GetActorRotation().UnrotateVector(ShooterCharacter->GetVelocity()));
	Direction = Movement.GetSafeNormal();
	State = ShooterCharacter->GetState();

	if (UPawnMovementComponent* MoveComp = ShooterCharacter->GetMovementComponent())
	{
		RelativeSpeed = Movement.Size() / MoveComp->GetMaxSpeed();
		UAnimationUtilities::UpdateMontagePlayState(this, ReloadMontage, State == EShooterCharacterState::Reload);

		if (ShooterCharacter->GetWeaponComponent())
		{
			float PlayRate = FireAnimLength / ShooterCharacter->GetWeaponComponent()->FireRate;

			UAnimationUtilities::UpdateMontagePlayState(this, FireMontage, ShooterCharacter->bIsShooting && State == EShooterCharacterState::IdleRun, PlayRate, .2f);
			UAnimationUtilities::UpdateMontagePlayState(this, FireAimMontage, ShooterCharacter->bIsShooting && State == EShooterCharacterState::Aim, PlayRate, .2f);
		}
	}
}

void UShooterCharacterAnim::PlayPushButtonMontage()
{
	if (!Montage_IsActive(PushMontage))
		Montage_Play(PushMontage);
}

void UShooterCharacterAnim::PlayPunchMontage()
{
	if (!Montage_IsActive(PunchMontage))
		Montage_Play(PunchMontage, 0.6f);
}

void UShooterCharacterAnim::PlayHitMontage()
{
	Montage_Play(HitMontage);
}

void UShooterCharacterAnim::AnimNotify_PunchHit(UAnimNotify* Notify) const
{
	if (ShooterCharacter && ShooterCharacter->IsLocallyControlled())
		ShooterCharacter->SR_InflictPunch(GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
}

void UShooterCharacterAnim::AnimNotify_WeaponReloaded(UAnimNotify* Notify) const
{
	if (ShooterCharacter)
		ShooterCharacter->ReloadWeapon();
}

void UShooterCharacterAnim::AnimNotify_PushButton(UAnimNotify* Notify) const
{
	if (ShooterCharacter)
		ShooterCharacter->SR_InflictPushButton();
}

void UShooterCharacterAnim::MontageEnded() const
{
	if (ShooterCharacter->HasAuthority())
	{
		if (ShooterCharacter->GetState() == EShooterCharacterState::PushButton ||
			ShooterCharacter->GetState() == EShooterCharacterState::Punch)
			ShooterCharacter->SetState(EShooterCharacterState::IdleRun);
	}
	else
	{
		if (ShooterCharacter->ShooterCharacterMovement->Safe_bWantsToReload)
			ShooterCharacter->EndWantsToReload();
	}
}
