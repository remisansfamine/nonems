// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyRoomController.h"

#include "GameFrameWork/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "ShooterMulti/GameFramework/LobbyRoomGM.h"

void ALobbyRoomController::Sr_GetAndVerifyPassword_Implementation(const FString& password)
{
	ALobbyRoomGM* GM =  GetWorld()->GetAuthGameMode<ALobbyRoomGM>();

	
	
	GM->CheckUserPassword(password, this);
}

FTransform ALobbyRoomController::GetFreePlayerStart(TArray<AActor*> PlayerStarts) const
{
	for (AActor* actor: PlayerStarts)
	{
		APlayerStart* PlayerStart = Cast<APlayerStart>(actor);
		if (!PlayerStart->ActorHasTag("Taken"))
		{
			PlayerStart->Tags = {"Taken"};
			return PlayerStart->GetTransform();
		}
	}

	return FTransform();
}

void ALobbyRoomController::CL_UpdateCharacters_Implementation(int CharactersCount)
{
	TArray<AActor*> SpawnedCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), LobbyCharacter, SpawnedCharacters);

	for (AActor* actor: SpawnedCharacters)
	{
		actor->Destroy();
	}

	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);
	
	PlayerStarts.Sort([](const AActor& lhs, const AActor& rhs) {
		return lhs.GetName() < rhs.GetName();
	});
	
	for (AActor* actor: PlayerStarts)
	{
		APlayerStart* PlayerStart = Cast<APlayerStart>(actor);
		PlayerStart->Tags = {};

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, PlayerStart->GetName());
	}

	for (int i = 0; i < CharactersCount; i++)
	{
		UClass* CharacterClass = LobbyCharacter.Get();
		FTransform CharacterTransform = GetFreePlayerStart(PlayerStarts);
		GetWorld()->SpawnActor(CharacterClass, &CharacterTransform);
	}
}