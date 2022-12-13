#include "EnemySpawnerButton.h"
#include "../AI/UndeadDirector.h"
#include "../GameFramework/DeathMatchGS.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AEnemySpawnerButton::AEnemySpawnerButton()
	: Super()
{
	bReplicates = true;
}

void AEnemySpawnerButton::BeginPlay()
{
	Super::BeginPlay();

	if (material == nullptr)
	{
		TArray<UStaticMeshComponent*> Components;
		GetComponents<UStaticMeshComponent>(Components);
		material = Components[0]->CreateAndSetMaterialInstanceDynamic(2);
	}
}

void AEnemySpawnerButton::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEnemySpawnerButton, mTeam);
}

void AEnemySpawnerButton::OnRep_TeamChange() const
{
	if (!material)
		return;

	TMap<ETeam, FLinearColor> ColorMap {
			{ ETeam::Blue, FLinearColor::Blue },
			{ ETeam::Red, FLinearColor::Red },
			{ ETeam::None, FLinearColor::Green },
			{ ETeam::AI, FLinearColor::Green }
	};
	
	material->SetVectorParameterValue("ColorActive", ColorMap[mTeam]);
}

void AEnemySpawnerButton::Activate(ETeam team)
{
	if (mTeam == team)
		return;
	
	mTeam = team;
	OnRep_TeamChange();
	
	auto lambda = [this]()
	{
		AUndeadDirector::GetInstance()->SpawnEnemy(GetActorLocation(), GetActorRotation(), mTeam);
	};
	
	GetWorld()->GetTimerManager().SetTimer(mSpawnTimerHandle, lambda, SecondPerSpawn, true);

	CL_OnActivate_Implementation();
}

void AEnemySpawnerButton::CL_OnActivate_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ActivateSound, GetActorLocation());
}

void AEnemySpawnerButton::Reset()
{
	Super::Reset();
	
	mTeam = ETeam::None;

	GetWorldTimerManager().ClearTimer(mSpawnTimerHandle);
}
