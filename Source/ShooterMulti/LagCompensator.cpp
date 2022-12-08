// Fill out your copyright notice in the Description page of Project Settings.


#include "LagCompensator.h"

#include "GameFramework/DeathMatchGS.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

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

void ALagCompensator::SR_ReplayFrame_Implementation(float TimeStamp)
{
	for (const FSavedCollider_Shooter& Frame : CollidersFrames)
	{
		if (CurrentTimeStamp > Frame.TimeStamp)
		{
			ApplyFrame(Frame);
			
			return;
		}
	}
}

void ALagCompensator::Replay()
{
	SR_ReplayFrame(CurrentTimeStamp);
}

void ALagCompensator::SaveFrame()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);

	FSavedCollider_Shooter Frame;

	Frame.TimeStamp = CurrentTimeStamp;

	for (UPrimitiveComponent* Collider : SubscribedPrimitives)
		Frame.CollidersStates.Add({Collider, Collider->GetComponentTransform()});

	CurrentFrame = Frame;
	
	CollidersFrames.Add(Frame);
}

void ALagCompensator::ApplyFrame(const FSavedCollider_Shooter& FrameToApply)
{
	for (auto& ColliderFrame : FrameToApply.CollidersStates)
		ColliderFrame.Collider->SetWorldTransform(ColliderFrame.Transform);
}

void ALagCompensator::SR_ResetFrame_Implementation()
{
	ApplyFrame(CurrentFrame);
}

void ALagCompensator::ResetFrame()
{
	SR_ResetFrame();
}

// Called every frame
void ALagCompensator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentTimeStamp = GetWorld()->RealTimeSeconds;

	if (GetLocalRole() == ROLE_Authority)
		SaveFrame();
}

void ALagCompensator::SubscribeReplication(const AActor* ActorToSubscribe)
{
	const TSet<UActorComponent*> ActorComponents = ActorToSubscribe->GetComponents();

	for (UActorComponent* ActorComponent : ActorComponents)
	{
		if (UPrimitiveComponent* Collider = Cast<UPrimitiveComponent>(ActorComponent))
			SubscribedPrimitives.Add(Collider);
	}
}

void ALagCompensator::UnsubscribeReplication(const AActor* ActorToUnsubscribe)
{
	TSet<UActorComponent*> ActorComponents = ActorToUnsubscribe->GetComponents();

	for (UActorComponent* ActorComponent : ActorComponents)
	{
		if (const UPrimitiveComponent* Collider = Cast<UPrimitiveComponent>(ActorComponent))
			SubscribedPrimitives.Remove(Collider);
	}
}