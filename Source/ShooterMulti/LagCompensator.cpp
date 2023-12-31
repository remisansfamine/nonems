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
	
	if (HasAuthority())
	{
		DeathMatchGS = GetWorld()->GetGameState<ADeathMatchGS>();
		if (DeathMatchGS)
			DeathMatchGS->SetLagCompensator(this);
	}
}

void ALagCompensator::SR_StartCompensation_Implementation(float TimeStamp)
{
	for (const LabelProfile& Profile : SubscribedLabels)
	{
		for (const FSavedFrame_Shooter& Frame : Profile.ComponentsFrames)
		{
			if (Frame.TimeStamp >= TimeStamp)
				return ApplyFrame(Frame);
		}
	}
}

void ALagCompensator::ClearOldFrames()
{
	for (LabelProfile& Profile : SubscribedLabels)
	{
		for (TArray<FSavedFrame_Shooter>::TIterator It(Profile.ComponentsFrames); It; ++It)
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

		TArray<FSavedFrame_Shooter::ComponentState> ComponentsStates;
		
		for (USceneComponent* Compensated : CompensatedComponents)
			ComponentsStates.Add({Compensated, Compensated->GetComponentTransform()});

		Profile.ComponentsFrames.Add({ CurrentTimeStamp, ComponentsStates });
	}
}

void ALagCompensator::ApplyFrame(const FSavedFrame_Shooter& FrameToApply)
{
	for (auto& ComponentFrame : FrameToApply.ComponentsStates)
		ComponentFrame.Component->SetWorldTransform(ComponentFrame.Transform, false, nullptr, ETeleportType::TeleportPhysics);
}

void ALagCompensator::SR_FinishCompensation_Implementation()
{
	for (const LabelProfile& Profile : SubscribedLabels)
	{
		if (Profile.ComponentsFrames.Num() > 0)
			ApplyFrame(Profile.ComponentsFrames.Last());
	}
}

// Called every frame
void ALagCompensator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentTimeStamp = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();

	if (HasAuthority())
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
	const int IndexToRemove = SubscribedLabels.IndexOfByPredicate([LabelToUnsubscribe](const LabelProfile& Profile)
	{
		return Profile.GetLabel() == LabelToUnsubscribe;
	});

	if (IndexToRemove != INDEX_NONE)
		SubscribedLabels.RemoveAtSwap(IndexToRemove, 1, false);
	
}