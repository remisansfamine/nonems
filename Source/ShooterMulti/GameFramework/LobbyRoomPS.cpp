// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyRoomPS.h"

#include "UtilsFunctionsLibrary.h"

FString& ALobbyRoomPS::GetClientSetup()
{
	UPlayerGI* GameInstance = GetWorld()->GetGameInstance<UPlayerGI>();

	ClientSetup.Name = GameInstance->GetUserInfo().Name;
	ClientSetup.Team = GameInstance->GetUserInfo().Team;

	SR_SetClientSetup(ClientSetup);

	return ClientSetup.Name;
}

void ALobbyRoomPS::SetPawn()
{
	if (Pawn)
	{
		Pawn->GetMesh()->SetVectorParameterValueOnMaterials("TeamColor", FVector((ClientSetup.Team != ETeam::Blue), (ClientSetup.Team != ETeam::Red && ClientSetup.Team != ETeam::Blue), (ClientSetup.Team != ETeam::Red)));
		Pawn->ChangeHeadName(ClientSetup.Name);
		Pawn->ChangeNameColor(FColor(!ClientSetup.bIsReady * 255, ClientSetup.bIsReady * 255, 0, 255));
	}
}

void ALobbyRoomPS::Rep_ClientDatas()
{
	Super::Rep_ClientDatas();
	
	if (Pawn && Pawn->IsLocallyControlled())
	{
		UPlayerGI* GameInstance = GetWorld()->GetGameInstance<UPlayerGI>();
		GameInstance->SetUserInfo(ClientSetup);
	}
	
	SetPawn();

	if (OnServerSetReady.IsBound())
		OnServerSetReady.Broadcast(ClientSetup.bIsReady);
}
