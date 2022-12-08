// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyRoomGS.h"

#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "ShooterMulti/Characters/LobbyRoomCharacter.h"

FTransform ALobbyRoomGS::GetFreePlayerStart() const
{
	const TArray<AActor*> PlayerStarts = GetSortedPlayerStarts();
	return GetFreePlayerStart(PlayerStarts);
}

FTransform ALobbyRoomGS::GetFreePlayerStart(const TArray<AActor*>& PlayerStarts) const
{
	// Dont check for the first player start (reserve for the self client)
	for (int i = 1; i < PlayerStarts.Num(); i++)
	{
		APlayerStart* PlayerStart = Cast<APlayerStart>(PlayerStarts[i]);
		if (!PlayerStart->ActorHasTag("Taken"))
		{
			PlayerStart->Tags = {"Taken"};
			return PlayerStart->GetTransform();
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, "Has Taken TAG");
		}
	}

	return FTransform();
}

TArray<AActor*> ALobbyRoomGS::GetSortedPlayerStarts() const
{
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

	PlayerStarts.Sort([](const AActor& lhs, const AActor& rhs) {
		return lhs.GetName() < rhs.GetName();
	});

	return PlayerStarts;
}

void ALobbyRoomGS::SpawnAllCharacters(const APlayerController* LocalController) const
{
	const TArray<AActor*> PlayerStarts = GetSortedPlayerStarts();

	if (PlayerStarts.Num() == 0)
		return;
	
	for (const APlayerState* PlayerState : PlayerArray)
	{
		// Get PlayerStart transform 
		const FTransform CharacterTransform = LocalController->PlayerState == PlayerState ?
			PlayerStarts[0]->GetTransform() : GetFreePlayerStart(PlayerStarts);
		
		// Spawn the character
		AActor* SpawnedActor = GetWorld()->SpawnActor(*LobbyCharacter, &CharacterTransform);

		ALobbyRoomCharacter* Character = Cast<ALobbyRoomCharacter>(SpawnedActor);

		if (LocalController->PlayerState == PlayerState)
			Character->ChangeHeadName("Michel");
		else
			Character->ChangeHeadName("Patrick");
	}
}

void ALobbyRoomGS::Multi_UpdateCharacters_Implementation(const APlayerController* NewController)
{
	// Not running on server
	if (IsRunningDedicatedServer())
		return;

	// Not spawning again self
	if (NewController)
		return;

	const FTransform NewPlayerTransform = GetFreePlayerStart();
	AActor* SpawnedActor = GetWorld()->SpawnActor(*LobbyCharacter, &NewPlayerTransform);

	ALobbyRoomCharacter* Character = Cast<ALobbyRoomCharacter>(SpawnedActor);
	Character->ChangeHeadName("Patrick");
}