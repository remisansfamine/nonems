// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupComponent.h"

#include "Kismet/GameplayStatics.h"
#include "ShooterMulti/LD/PickupDirector.h"

// Sets default values for this component's properties
UPickupComponent::UPickupComponent()
{
}

// Called when the game starts
void UPickupComponent::BeginPlay()
{
	Super::BeginPlay();

	// Dont register if not authority (solo play or server)
	if (!GetOwner()->HasAuthority())
		return;

	// ...
	AActor* DirectorActor = UGameplayStatics::GetActorOfClass(GetWorld(), APickupDirector::StaticClass());
	APickupDirector* Director = Cast<APickupDirector>(DirectorActor);

	if (Director)
		Director->RegisterPickup(*this);
}