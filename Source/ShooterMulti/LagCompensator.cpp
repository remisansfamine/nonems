// Fill out your copyright notice in the Description page of Project Settings.


#include "LagCompensator.h"

#include "GameFramework/DeathMatchGS.h"
#include "Kismet/GameplayStatics.h"
#include "Engine.h"

// Sets default values
ALagCompensator::ALagCompensator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


}

// Called when the game starts or when spawned
void ALagCompensator::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetLocalRole() == ROLE_Authority)
	{
		if (ADeathMatchGS* GS = Cast<ADeathMatchGS>(GetWorld()->GetGameState()))
			GS->SetLagCompensator(this);
	}
}

void ALagCompensator::SR_StartCompensation_Implementation(float TimeStamp)
{
	for (const FSavedComponent_Shooter& Frame : ComponentsFrames)
	{
		if (Frame.TimeStamp >= TimeStamp)
		{
			ApplyFrame(Frame);
			
			return;
		}
	}
}

void ALagCompensator::SaveFrame()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);

	FSavedComponent_Shooter Frame;

	Frame.TimeStamp = CurrentTimeStamp;

	for (UCompensatorLabel* Compensator : SubscribedLabels)
	{
		for (USceneComponent* Compensated : Compensator->CompensatedComponents)
			Frame.ComponentsStates.Add({Compensated, Compensated->GetComponentTransform()});
	}

	ComponentsFrames.Add(Frame);
}

void ALagCompensator::ApplyFrame(const FSavedComponent_Shooter& FrameToApply)
{
	for (auto& ComponentFrame : FrameToApply.ComponentsStates)
		ComponentFrame.Component->SetWorldTransform(ComponentFrame.Transform);
}

void ALagCompensator::SR_FinishCompensation_Implementation()
{
	ApplyFrame(ComponentsFrames.Last());
}

// Called every frame
void ALagCompensator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentTimeStamp = GetWorld()->RealTimeSeconds;

	if (GetLocalRole() == ROLE_Authority)
		SaveFrame();
}

void ALagCompensator::SubscribeLabel(UCompensatorLabel* LabelToSubscribe)
{
	SubscribedLabels.Add(LabelToSubscribe);
}

void ALagCompensator::UnsubscribeLabel(UCompensatorLabel* LabelToUnsubscribe)
{
	SubscribedLabels.Remove(LabelToUnsubscribe);
}