// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyRoomGS.h"

#include "Components/SplineComponent.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "ShooterMulti/Characters/LobbyRoomCharacter.h"

FTransform ALobbyRoomGS::GetFreePlayerStart() const
{
	// Dont check for the first player start (reserve for the self client)
	for (int i = 1; i < PlayerStarts.Num(); i++)
	{
		APlayerStart* PlayerStart = Cast<APlayerStart>(PlayerStarts[i]);
		if (PlayerStart->PlayerStartTag != "TakenTag")
		{
			PlayerStart->PlayerStartTag = FName("TakenTag");
			return PlayerStart->GetTransform();
		}
		
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, "Has Taken TAG");
	}

	return FTransform();
}

void ALobbyRoomGS::SetupPlayerStart()
{
	// Get all PlayerStart actors
	TArray<AActor*> PlayerStartActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStartActors);

	// Check if found
	if (PlayerStartActors.Num() == 0)
		return;

	// Sort all player starts
	PlayerStartActors.Sort([](const AActor& lhs, const AActor& rhs) {
		return lhs.GetName() < rhs.GetName();
	});

	// Get the front player start
	SelfPlayerStart = Cast<APlayerStart>(PlayerStartActors[0]);

	// Add the others to the available player start list
	for (int i = 1; i < PlayerStartActors.Num(); i++)
	{
		PlayerStarts.Add(Cast<APlayerStart>(PlayerStartActors[i]));
	}
}

void ALobbyRoomGS::BeginPlay()
{
	Super::BeginPlay();

	SetupPlayerStart();
}

void ALobbyRoomGS::SpawnSelfCharacter(APlayerController* LocalController)
{
	// Get the self PlayerStart transform
	const FTransform CharacterTransform = SelfPlayerStart->GetTransform();
		
	// Spawn the character
	AActor* SpawnedActor = GetWorld()->SpawnActor(*LobbyCharacter, &CharacterTransform);

	// Get the associated character
	ALobbyRoomCharacter* Character = Cast<ALobbyRoomCharacter>(SpawnedActor);

	// Setup the character
	Character->PossessedBy(LocalController);
	Character->ChangeHeadName("Michel");

	SelfController = LocalController;
}

void ALobbyRoomGS::UpdateCharacters()
{
	if (IsRunningDedicatedServer() || !SelfController)
		return;
	
	if ((PlayerArray.Num() - 1) == CharactersSpawned.Num())
		return;

	// Destroy all characters
	for (ACharacter* Character : CharactersSpawned)
		Character->Destroy();

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, "Update characters");
	
	// Clean the list
	CharactersSpawned.Empty();

	// Respawn characters
	for (int i = 1; i < PlayerArray.Num(); i++)
	{
		// Get the self PlayerStart transform
		const FTransform CharacterTransform = (i - 1) < PlayerStarts.Num() ?
			PlayerStarts[i - 1]->GetTransform() : SelfPlayerStart->GetTransform();
		
		// Spawn the character
		AActor* SpawnedActor = GetWorld()->SpawnActor(*LobbyCharacter, &CharacterTransform);
		
		// Get the associated character
		ALobbyRoomCharacter* Character = Cast<ALobbyRoomCharacter>(SpawnedActor);

		// Setup the character
		Character->ChangeHeadName("Patrick");

		// Add the character to the list
		CharactersSpawned.Add(Character);
	}
}

void ALobbyRoomGS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ALobbyRoomGS::Multi_UpdateCharacters_Implementation(const APlayerController* NewController)
{
	return;
	
	// Not running on server
	if (IsRunningDedicatedServer())
	{
		UE_LOG(LogTemp, Warning, TEXT("Server dont spawn character"));
		return;
	}

	// Not spawning again self
	if (NewController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not spawning self"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Spawning character SINGLE"));

	const FTransform NewPlayerTransform = GetFreePlayerStart();
	AActor* SpawnedActor = GetWorld()->SpawnActor(*LobbyCharacter, &NewPlayerTransform);

	ALobbyRoomCharacter* Character = Cast<ALobbyRoomCharacter>(SpawnedActor);
	Character->ChangeHeadName("Patrick");
}
