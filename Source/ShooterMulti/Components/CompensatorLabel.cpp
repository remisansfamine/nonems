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
	
	if (GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		if (ADeathMatchGS* GS = GetWorld()->GetGameState<ADeathMatchGS>())
			GS->GetLagCompensator()->SubscribeReplication(GetOwner());
	}
}

void UCompensatorLabel::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EndPlayReason != EEndPlayReason::Type::Destroyed)
		return;
	
	if (GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		if (UWorld* World = GetWorld())
		if (ADeathMatchGS* GS = World->GetGameState<ADeathMatchGS>())
			GS->GetLagCompensator()->UnsubscribeReplication(GetOwner());
	}
}
