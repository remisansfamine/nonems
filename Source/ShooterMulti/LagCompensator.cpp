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
	
	if (ADeathMatchGS* GS = Cast<ADeathMatchGS>(GetWorld()->GetGameState()))
		GS->SetLagCompensator(this);
}

void ALagCompensator::SaveFrame()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);

	FSavedCollider_Shooter Frame;

	Frame.TimeStamp = GetWorld()->RealTimeSeconds;

	for (const UPrimitiveComponent* Collider : SubsribedPrimitives)
		Frame.ColliderMap[Collider] = Collider->GetComponentTransform();

	CollidersFrame.Add(Frame);
}

// Called every frame
void ALagCompensator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() == ROLE_Authority)
		SaveFrame();
}

void ALagCompensator::SubscribeReplication(const AActor* ActorToSubscribe)
{
	const TSet<UActorComponent*> ActorComponents = ActorToSubscribe->GetComponents();

	for (UActorComponent* ActorComponent : ActorComponents)
	{
		if (UPrimitiveComponent* Collider = Cast<UPrimitiveComponent>(ActorComponent))
			SubsribedPrimitives.Add(Collider);
	}
}

void ALagCompensator::UnsubscribeReplication(const AActor* ActorToUnsubscribe)
{
	TSet<UActorComponent*> ActorComponents = ActorToUnsubscribe->GetComponents();

	for (UActorComponent* ActorComponent : ActorComponents)
	{
		if (const UPrimitiveComponent* Collider = Cast<UPrimitiveComponent>(ActorComponent))
			SubsribedPrimitives.Remove(Collider);
	}
}