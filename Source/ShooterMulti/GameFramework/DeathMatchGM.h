#pragma once

#include "GameFramework/GameModeBase.h"
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
	
	UFUNCTION(BlueprintCallable)
	void Respawn(APlayerController* PlayerController);

	void Quit();
};
