// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyRoomGS.h"

#include "EngineUtils.h"
#include "InteractiveToolManager.h"
#include "LobbyRoomPS.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
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
		if (i < 5)
			RightPlayerStarts.Add(Cast<APlayerStart>(PlayerStartActors[i]));
		else if (i < 10)
			LeftPlayerStarts.Add(Cast<APlayerStart>(PlayerStartActors[i]));
		else
			NonePlayerStarts.Add(Cast<APlayerStart>(PlayerStartActors[i]));
	}
}

void ALobbyRoomGS::SetNewCurrentMap(const FString& MapName, const FString& MapPath)
{
	FMapPreviewSetup MapSetup;

	MapSetup.Name = MapName;
	MapSetup.Path = MapPath;

	CurrentMapSetup = MapSetup;
}

void ALobbyRoomGS::Multi_OnSwitchTeam_Implementation()
{
	if (IsRunningDedicatedServer())
		return;
	
	OnSwitchTeam();
}

void ALobbyRoomGS::OnSwitchTeam()
{
	bClientHasSwitchTeam = true;
}

void ALobbyRoomGS::OnRep_SetNewCurrentMap()
{
	CurrentMapPreviewName = CurrentMapSetup.Name;

	// Dont always reload the texture (store it)
	CurrentMapPreviewTexture = LoadObject<UTexture2D>(GetTransientPackage(), *CurrentMapSetup.Path);

	// Action on rep
	if (MapChangeDelegate.IsBound())
		MapChangeDelegate.Broadcast();
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
	
	if (((PlayerArray.Num() - 1) == CharactersSpawned.Num()) && !bClientHasSwitchTeam)
		return;

	bClientHasSwitchTeam = false;

	const ALobbyRoomPS* SelfPlayerState = Cast<ALobbyRoomPS>(SelfController->PlayerState);

	// Destroy all characters
	for (ACharacter* Character : CharactersSpawned)
		Character->Destroy();
	
	// Clean the list
	CharactersSpawned.Empty();

	int NoneStartIndex = 0;
	int RightStartsIndex = 0;
	int LeftStartsIndex = 0;
	
	// Respawn characters
	for (int i = 1; i < PlayerArray.Num(); i++)
	{
		const ALobbyRoomPS* ClientPlayerState = Cast<ALobbyRoomPS>(PlayerArray[i]);
		FTransform ClientCharacterTransform;

		if (SelfPlayerState->ClientSetup.Team == ETeam::None || ClientPlayerState->ClientSetup.Team == ETeam::None)
		{
			ClientCharacterTransform = NonePlayerStarts[NoneStartIndex]->GetTransform();
			NoneStartIndex++;
		}
		else if (ClientPlayerState->ClientSetup.Team == SelfPlayerState->ClientSetup.Team)
		{
			ClientCharacterTransform = RightPlayerStarts[RightStartsIndex]->GetTransform();
			RightStartsIndex++;
		}
		else
		{
			ClientCharacterTransform = LeftPlayerStarts[LeftStartsIndex]->GetTransform();
			LeftStartsIndex++;
		}
		
		// Spawn the character
		AActor* SpawnedActor = GetWorld()->SpawnActor(*LobbyCharacter, &ClientCharacterTransform);
		
		// Get the associated character
		ALobbyRoomCharacter* Character = Cast<ALobbyRoomCharacter>(SpawnedActor);

		ALobbyRoomPS* CurrentPlayerState = static_cast<ALobbyRoomPS*>(PlayerArray[i]);

		CurrentPlayerState->Pawn = Character;
		CurrentPlayerState->SetPawn();
		
		// Add the character to the list
		CharactersSpawned.Add(Character);
	}
}

void ALobbyRoomGS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALobbyRoomGS, CurrentMapSetup);
}