#pragma once

#include "DeathMatchGM.h"
#include "PlayerGI.h"
#include "ShooterMultiPS_Base.h"
#include "GameFramework/GameStateBase.h"
#include "ShooterMulti/LagCompensator.h"
#include "DeathMatchGS.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateScores);

UCLASS()
class SHOOTERMULTI_API ADeathMatchGS : public AGameStateBase
{
	GENERATED_BODY()

	UPlayerGI* GameInstance;

	UPROPERTY()
	TArray<APlayerStart*> RedTeamPlayerStarts;
	UPROPERTY()
	TArray<APlayerStart*> BlueTeamPlayerStarts;
	
protected:
	UPROPERTY()
	ALagCompensator* CompensatorInstance = nullptr;
	
	FTimerHandle CountdownTimerHandle;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Shooter|GameState")
	ADeathMatchGM* GameMode = nullptr;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Shooter|GameState")
	int32 CurrentTime;
	UPROPERTY(BlueprintReadOnly, Category = "Shooter|GameState")
	int32 CurrentAICount = 0;
	UPROPERTY(ReplicatedUsing = Rep_TeamScores, BlueprintReadOnly, Category = "Shooter|GameState")
	int32 RedTeamScore = 0;
	UPROPERTY(ReplicatedUsing = Rep_TeamScores, BlueprintReadOnly, Category = "Shooter|GameState")
	int32 BlueTeamScore = 0;
	UPROPERTY(ReplicatedUsing = Rep_TeamScores, BlueprintReadOnly, Category = "Shooter|GameState")
	int32 MaxTeamScore = 0;

	void AdvanceTimer();

	void SetTeamsPlayerStarts();

	AActor* FindValidPlayerStart(AController* Controller, const TArray<APlayerStart*>& PlayerStartList);

	UFUNCTION()
	void Reset();

public:
	UPROPERTY(BlueprintAssignable)
	FUpdateScores OnScoresUpdated;

	UFUNCTION()
	void Rep_TeamScores();
	
	DECLARE_EVENT_OneParam(ADeathMatchGS, FOnPlayerAddAndRemove, ADeathMatchGS*)
	FOnPlayerAddAndRemove OnPlayerNum;
	DECLARE_EVENT_OneParam(ADeathMatchGS, TeamWinEvent, ETeam)
	TeamWinEvent OnTeamWin;
	DECLARE_EVENT(ADeathMatchGS, GameRestartEvent)
	GameRestartEvent OnGameRestart;
	GameRestartEvent OnResetAfterDelay;

	void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetScore(ETeam Team, int32 NewScore);
	int32 GetScore(ETeam Team) const;
	void AddPlayerState(APlayerState* PlayerState) override;
	void RemovePlayerState(APlayerState* PlayerState) override;

	bool CanAddAI();
	void AddAI();
	void RemoveAI();

	int GetNbplayer();

	void UpdateEndHud(ETeam Team);

	UFUNCTION(BlueprintCallable)
	void EndGameTrigg();

	UFUNCTION(BlueprintCallable)
	void ResetAfterDelay();

	UFUNCTION(BlueprintNativeEvent)
	void ShowTeamWinHUD(ETeam Team);
	void ShowTeamWinHUD_Implementation(ETeam Team) {};

	AActor* GetValidPlayerStart(AController* Controller);
	
	int NewFrequency(int Sec);

	void SetLagCompensator(ALagCompensator* Compensator) { if (Compensator && !CompensatorInstance) CompensatorInstance = Compensator; }
	ALagCompensator* GetLagCompensator() const { return CompensatorInstance; }
};
