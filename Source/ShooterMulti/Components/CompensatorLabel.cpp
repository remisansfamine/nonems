// Fill out your copyright notice in the Description page of Project Settings.


#include "CompensatorLabel.h"

#include "ShooterMulti/GameFramework/DeathMatchGS.h"

// Sets default values for this component's properties
UCompensatorLabel::UCompensatorLabel()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UCompensatorLabel::BeginPlay()
{
	Super::BeginPlay();
	
	if (!GetOwner()->HasAuthority())
		return;
	
	SubscribeReplication<UPrimitiveComponent>(GetOwner());
	
	if (ADeathMatchGS* GS = GetWorld()->GetGameState<ADeathMatchGS>())
		GS->GetLagCompensator()->SubscribeLabel(this);
}

void UCompensatorLabel::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EndPlayReason != EEndPlayReason::Type::Destroyed || !GetOwner()->HasAuthority())
		return;
	
	if (ADeathMatchGS* GS = GetWorld()->GetGameState<ADeathMatchGS>())
		GS->GetLagCompensator()->UnsubscribeLabel(this);
}
