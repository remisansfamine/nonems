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
	for (const LabelProfile& Profile : SubscribedLabels)
	{
		for (const FSavedComponent_Shooter& Frame : Profile.ComponentsFrames)
		{
			if (Frame.TimeStamp >= TimeStamp)
			{
				ApplyFrame(Frame);
			
				return;
			}
		}
	}
}

void ALagCompensator::ClearOldFrames()
{
	for (LabelProfile& Profile : SubscribedLabels)
	{
		for (TArray<FSavedComponent_Shooter>::TIterator It(Profile.ComponentsFrames); It; ++It)
		{
			if (CurrentTimeStamp - It->TimeStamp < MaxTimeStampOffset)
				break;
			
			It.RemoveCurrent();
		}
	}
}

void ALagCompensator::SaveFrame()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);

	for (LabelProfile& Profile : SubscribedLabels)
	{
		// TODO: Add TransformUpdated event
		
		const TArray<USceneComponent*>& CompensatedComponents = Profile.GetLabel()->CompensatedComponents;

		TArray<FSavedComponent_Shooter::ComponentState> ComponentsStates;
		
		for (USceneComponent* Compensated : CompensatedComponents)
			ComponentsStates.Add({Compensated, Compensated->GetComponentTransform()});

		Profile.ComponentsFrames.Add({ CurrentTimeStamp, ComponentsStates });
	}
}

void ALagCompensator::ApplyFrame(const FSavedComponent_Shooter& FrameToApply)
{
	for (auto& ComponentFrame : FrameToApply.ComponentsStates)
		ComponentFrame.Component->SetWorldTransform(ComponentFrame.Transform);
}

void ALagCompensator::SR_FinishCompensation_Implementation()
{
	for (const LabelProfile& Profile : SubscribedLabels)
		ApplyFrame(Profile.ComponentsFrames.Last());
}

// Called every frame
void ALagCompensator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentTimeStamp = GetWorld()->RealTimeSeconds;

	if (GetLocalRole() == ROLE_Authority)
	{
		ClearOldFrames();
		SaveFrame();
	}
}

void ALagCompensator::SubscribeLabel(UCompensatorLabel* LabelToSubscribe)
{
	SubscribedLabels.Add(LabelToSubscribe);
}

void ALagCompensator::UnsubscribeLabel(UCompensatorLabel* LabelToUnsubscribe)
{
	const LabelProfile* ProfilePtr = SubscribedLabels.FindByPredicate([LabelToUnsubscribe](const LabelProfile& Profile)
	{
		return Profile.GetLabel() == LabelToUnsubscribe;
	});

	if (ProfilePtr)
		SubscribedLabels.Remove(*ProfilePtr);
}