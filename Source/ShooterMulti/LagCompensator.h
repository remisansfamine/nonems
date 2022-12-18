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
	class FSavedFrame_Shooter 
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
	
	class LabelProfile
	{
		UCompensatorLabel* Label = nullptr;
		
	public:
		TArray<FSavedFrame_Shooter> ComponentsFrames;
		
		LabelProfile(UCompensatorLabel* Label) : Label(Label) { }
		
		UCompensatorLabel* GetLabel() const { return Label; }

		bool operator==(const LabelProfile& Other) const { return Label == Other.Label;}
	};

	TArray<LabelProfile> SubscribedLabels;
	
	float CurrentTimeStamp = 0.f;

	UPROPERTY()
	class ADeathMatchGS* DeathMatchGS = nullptr;
	
	void ClearOldFrames();
	void SaveFrame();
	static void ApplyFrame(const FSavedFrame_Shooter& FrameToApply);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	UPROPERTY(EditAnywhere)
	float MaxTimeStampOffset = 2.f;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Server, Reliable)
	virtual void SR_StartCompensation(float TimeStamp);
	
	UFUNCTION(Server, Reliable)
	virtual void SR_FinishCompensation();

	virtual void SubscribeLabel(UCompensatorLabel* LabelToSubscribe);
	virtual void UnsubscribeLabel(UCompensatorLabel* LabelToUnsubscribe);
};
