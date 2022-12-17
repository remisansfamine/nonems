#pragma once

#include "GameFramework/GameModeBase.h"
#include "GameFrameWork/PlayerStart.h"
#include "DeathMatchGM.generated.h"


UCLASS()
class SHOOTERMULTI_API ADeathMatchGM : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float InvincibilityTime = 3.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 GameTime = 300;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MaxKill = 20;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MaxAIPerPlayer = 10;

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 ScoreFromEnemyAIDeath = 15;	// When a Player kills an enemy AI

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 ScoreFromAllyAIKill = 10;   // When an ally AI kills an enemy Player

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 ScoreFromAllyAIDeath = -25; // When a Player team-kills an ally AI
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 ScoreFromAllyPlayerKill = -200; // When a Player team-kills an ally Player

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 ScoreFromEnemyPlayerKill = 150; // When a Player kills an enemy Player

	UFUNCTION(BlueprintCallable)
	void Respawn(APlayerController* PlayerController);

	void Quit();

	void UpdateScoreOnDeath(class AHealthCharacter* Killed, class AHealthCharacter* Killer) const;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
};
