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
		struct ColliderState
		{
			UPrimitiveComponent* Collider;
			FTransform Transform;
		};
		
		float TimeStamp;
		TArray<ColliderState> CollidersStates;
	};
	
	TArray<FSavedCollider_Shooter> CollidersFrames;
	FSavedCollider_Shooter CurrentFrame;

	UPROPERTY()
	TSet<UPrimitiveComponent*> SubscribedPrimitives;
	
	void SaveFrame();
	static void ApplyFrame(const FSavedCollider_Shooter& FrameToApply);
	
	float CurrentTimeStamp = 0.f;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Server, Reliable)
	virtual void SR_StartCompensation(float TimeStamp);
	
	UFUNCTION(Server, Reliable)
	virtual void SR_FinishCompensation();

	void SubscribeReplication(const AActor* ActorToSubscribe);
	void UnsubscribeReplication(const AActor* ActorToUnsubscribe);

};
