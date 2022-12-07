// Fill out your copyright notice in the Description page of Project Settings.


#include "CompensatorLabel.h"

#include "ChaosInterfaceWrapperCore.h"
#include "IDetailTreeNode.h"
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

	if (ADeathMatchGS* GS = Cast<ADeathMatchGS>(GetWorld()->GetGameState()))
		GS->GetLagCompensator()->SubscribeReplication(GetOwner());
}

void UCompensatorLabel::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ADeathMatchGS* GS = Cast<ADeathMatchGS>(GetWorld()->GetGameState()))
		GS->GetLagCompensator()->UnsubscribeReplication(GetOwner());
}
