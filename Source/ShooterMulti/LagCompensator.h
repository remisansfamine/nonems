// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LagCompensator.generated.h"

UCLASS()
class SHOOTERMULTI_API ALagCompensator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALagCompensator();

private:
	class FSavedCollider_Shooter 
	{
	public:
		float TimeStamp;
		TMap<UPrimitiveComponent*, FTransform> ColliderMap;
	};
	
	TArray<FSavedCollider_Shooter> CollidersFrame;
	FSavedCollider_Shooter CurrentFrame;

	UPROPERTY()
	TSet<UPrimitiveComponent*> SubsribedPrimitives;
	
	void SaveFrame();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SubscribeReplication(const AActor* ActorToSubscribe);
	void UnsubscribeReplication(const AActor* ActorToUnsubscribe);

};
