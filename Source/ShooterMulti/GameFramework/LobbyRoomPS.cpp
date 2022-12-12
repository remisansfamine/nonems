// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyRoomPS.h"

FString& ALobbyRoomPS::GetClientSetup()
{
	UPlayerGI* GameInstance = GetWorld()->GetGameInstance<UPlayerGI>();

	ClientSetup.Name = GameInstance->UserName;

	SR_SetClientSetup(ClientSetup);

	return GameInstance->UserName;
}

void ALobbyRoomPS::Rep_ClientDatas()
{
	Super::Rep_ClientDatas();

	if (Pawn)
	{
		Pawn->GetMesh()->SetVectorParameterValueOnMaterials("TeamColor", FVector((ClientSetup.Team != ETeam::Blue), (ClientSetup.Team != ETeam::Red && ClientSetup.Team != ETeam::Blue), (ClientSetup.Team != ETeam::Red)));
		Pawn->ChangeHeadName(ClientSetup.Name);
	}
}
