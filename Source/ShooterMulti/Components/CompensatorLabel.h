// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CompensatorLabel.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTERMULTI_API UCompensatorLabel : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCompensatorLabel();

	UPROPERTY()
	TArray<USceneComponent*> CompensatedComponents;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	
	template <typename ComponentT>
	void SubscribeReplication(const AActor* ActorToSubscribe)
	{
		const TSet<UActorComponent*> ActorComponents = ActorToSubscribe->GetComponents();

		for (UActorComponent* ActorComponent : ActorComponents)
		{
			if (ComponentT* Component = Cast<ComponentT>(ActorComponent))
				CompensatedComponents.Add(Component);
		}
	}

	template <typename ComponentT>
	void UnsubscribeReplication(const AActor* ActorToUnsubscribe)
	{
		TSet<UActorComponent*> ActorComponents = ActorToUnsubscribe->GetComponents();

		for (UActorComponent* ActorComponent : ActorComponents)
		{
			// Keep only colliders
			if (const ComponentT* Component = Cast<ComponentT>(ActorComponent))
				CompensatedComponents.Remove(Component);
		}
	}
};
