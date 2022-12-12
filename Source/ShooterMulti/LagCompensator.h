// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CompensatorLabel.h"
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
	TSet<UCompensatorLabel*> SubscribedLabels;
	
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

	virtual void SubscribeLabel(UCompensatorLabel* LabelToSubscribe);
	virtual void UnsubscribeLabel(UCompensatorLabel* LabelToUnsubscribe);
};
