// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyRoomCharacter.h"

// Sets default values
ALobbyRoomCharacter::ALobbyRoomCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = false;

	HeadNameText = CreateDefaultSubobject<UTextRenderComponent>("HeadNameText");
	HeadNameText->SetTextRenderColor(FColor::White);
	HeadNameText->SetWorldScale3D(FVector(1.f, 1.f, 1.f));
	HeadNameText->SetUsingAbsoluteRotation(true);
	HeadNameText->Text = FText::FromString("Name");

	const FAttachmentTransformRules attachedRules = FAttachmentTransformRules(EAttachmentRule::KeepRelative, false);
	HeadNameText->AttachToComponent(RootComponent, attachedRules);
	
	ConstructorHelpers::FObjectFinder<USkeletalMesh> SKMeshContainer(TEXT("SkeletalMesh'/Game/Resources/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (SKMeshContainer.Succeeded())
		GetMesh()->SetSkeletalMesh(SKMeshContainer.Object);
	
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	
	// Create Weapon
	Weapon = CreateDefaultSubobject<UWeaponComponent>("Rifle");

	ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshContainer(TEXT("SkeletalMesh'/Game/Weapons/Rifle.Rifle'"));
	if (MeshContainer.Succeeded())
		Weapon->SetSkeletalMesh(MeshContainer.Object);

	Weapon->SetRelativeLocation(FVector(1.0f, 4.0f, -2.0f));
	Weapon->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	Weapon->SetupAttachment(GetMesh(), "hand_r");
}

// Called when the game starts or when spawned
void ALobbyRoomCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ALobbyRoomCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ALobbyRoomCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ALobbyRoomCharacter::ChangeHeadName(const FString& Name)
{
	if (HeadNameText)
		HeadNameText->SetText(FText::FromString(Name));
}

void ALobbyRoomCharacter::ChangeNameColor(const FColor& InColor)
{
	if (HeadNameText)
		HeadNameText->SetTextRenderColor(InColor);
}
