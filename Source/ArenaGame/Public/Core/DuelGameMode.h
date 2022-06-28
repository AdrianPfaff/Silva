// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "ArenaPlayerController.h"
#include "GameModeInterface.h"
#include "GameFramework/GameModeBase.h"
#include "DuelGameMode.generated.h"

class ADuelPlayerStart;
class AArenaDoor;
class AArenaPlayerController;

UENUM()
enum class EArena
{
	A,
	B,
	None
};

/**
 * @brief captures current duels in progress
 */
USTRUCT()
struct FDuelPlayerState
{
	GENERATED_BODY()

	UPROPERTY()
	AArenaPlayerController* Controller=nullptr;

	UPROPERTY()
	AArenaPlayerController* Opponent=nullptr;

	uint8 Wins=0;

	bool bInDuel=false;

	EArena InArena=EArena::None;

	explicit FDuelPlayerState(AArenaPlayerController* InController) : Controller(InController) {}
	
	FDuelPlayerState() = default;
	
};

/**
 * @brief Game mode for the arena to handle duels between two players
 */
UCLASS()
class ARENAGAME_API ADuelGameMode : public AGameModeBase, public IGameModeInterface
{
	GENERATED_BODY()

public:
	ADuelGameMode();
	
	virtual void BeginPlay() override;

	virtual void GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList) override;

	/** called from arena starts to register locations and indexes */
	void RegisterSpawn(ADuelPlayerStart* PlayerStart, uint8 StartIndex);
	/* called from arena doors to allow triggering opening */
	void RegisterDoor(AArenaDoor* Door, uint8 DoorIndex);

	virtual void GenericPlayerInitialization(AController* C) override;

	/* handles player death/losing an duel */
	virtual void HandlePlayerDeath(ACharacter* DeadPlayer, float& OutRespawnDelay) override;

protected:
	
	/**
	 * @brief Time in seconds to wait before beginning match
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxWaitTime=30.f;

	/**
	 * @brief Time in seconds each player can prepare before the doors open
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PreparationTime=30.f;

	/** Delay before the player should respawn */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float RespawnDelay=5.f;

	

private:
	/* handle for evaluating duels */
	FTimerHandle DuelHandle;

	/** Players in the game with additional data*/
	UPROPERTY()
	TArray<FDuelPlayerState> Players;

	UPROPERTY()
	TArray<AArenaDoor*> Doors;

	UPROPERTY()
	TArray<ADuelPlayerStart*> DuelPlayerStarts;

	/** Initializes everything required to evaluate duels */
	void InitializeDuels();
	
	/** Determines next step to take*/
	void EvaluateDuels();

	/** Starts a duel in the given arena */
	void BeginDuel(EArena InArena, FDuelPlayerState& PlayerA, FDuelPlayerState& PlayerB);

	/** Ends a duel in the given arena. Called from HandlePlayerDeath() to clean up and prepare next step */
	void EndDuel(EArena InArena, FDuelPlayerState& Winner, FDuelPlayerState& Loser);

	/** counts fights remaining internally */
	uint8 DuelsLeft=0;


	bool bArenaAInProgress=false;
	bool bArenaBInprogress=false;

	/** returns the first player not currently occupied otherwise */
	FDuelPlayerState* GetNextAvailablePlayer(FDuelPlayerState* StateToIgnore=nullptr);

	/** finds a specific player in the saved states */
	FDuelPlayerState* FindPlayer(AArenaPlayerController* InController);

	/** Handles win after all duels have concluded */
	void HandleWin();
};






