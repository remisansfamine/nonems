// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyRoomGS.h"

#include "LobbyRoomPS.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "ShooterMulti/Characters/LobbyRoomCharacter.h"

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

	ALobbyRoomPS* PlayerState = LocalController->GetPlayerState<ALobbyRoomPS>();

	// Setup the character
	PlayerState->Pawn = Character;

	const FString& Name = PlayerState->GetClientSetup();
	
	Character->PossessedBy(LocalController);	
	Character->ChangeHeadName(Name);
	
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

		ALobbyRoomPS* CurrentPlayerState = static_cast<ALobbyRoomPS*>(PlayerArray[i]);

		CurrentPlayerState->Pawn = Character;
		
		// Setup the character
		Character->ChangeHeadName(CurrentPlayerState->ClientSetup.Name);
		
		// Add the character to the list
		CharactersSpawned.Add(Character);
	}
}