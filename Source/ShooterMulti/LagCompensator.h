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
	class FSavedComponent_Shooter 
	{
	public:
		struct ComponentState
		{
			USceneComponent* Component;
			FTransform Transform;
		};
		
		float TimeStamp;
		TArray<ComponentState> ComponentsStates;
	};
	
	TArray<FSavedComponent_Shooter> ComponentsFrames;

	UPROPERTY()
	TSet<USceneComponent*> SubscribedComponents;
	
	void SaveFrame();
	static void ApplyFrame(const FSavedComponent_Shooter& FrameToApply);
	
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

	template <typename ComponentT>
	void SubscribeReplication(const AActor* ActorToSubscribe)
	{
		const TSet<UActorComponent*> ActorComponents = ActorToSubscribe->GetComponents();

		for (UActorComponent* ActorComponent : ActorComponents)
		{
			if (ComponentT* Collider = Cast<ComponentT>(ActorComponent))
				SubscribedComponents.Add(Collider);
		}
	}

	template <typename ComponentT>
	void UnsubscribeReplication(const AActor* ActorToUnsubscribe)
	{
		TSet<UActorComponent*> ActorComponents = ActorToUnsubscribe->GetComponents();

		for (UActorComponent* ActorComponent : ActorComponents)
		{
			// Keep only colliders
			if (const UPrimitiveComponent* Collider = Cast<UPrimitiveComponent>(ActorComponent))
				SubscribedComponents.Remove(Collider);
		}
	}
};
