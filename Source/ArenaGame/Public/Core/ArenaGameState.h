// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "ArenaPlayerCharacter.h"
#include "GameFramework/GameStateBase.h"
#include "ArenaGameState.generated.h"


class AArenaPlayerController;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDungeonPhaseEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDungeonPhaseBegin, FTimespan, Duration);

/**
 * @brief Structure containing association between the player mesh and controller to correctly sync meshes in online play
 */
USTRUCT()
struct FPlayerPair
{
	GENERATED_BODY()

	UPROPERTY()
	AArenaPlayerController* Player;
	UPROPERTY()
	USkeletalMesh* Mesh;

	FPlayerPair(AArenaPlayerController* InPlayer, USkeletalMesh* InMesh) : Player(InPlayer), Mesh(InMesh) {}

	FPlayerPair() = default;
};
/**
 * @brief Contains all game state data relevant to the running match
 */
UCLASS()
class ARENAGAME_API AArenaGameState : public AGameStateBase
{
	GENERATED_BODY()


public:
	/* Called Right before Traveling to Arena */
	UPROPERTY(BlueprintAssignable)
	FDungeonPhaseEnded OnDungeonPhaseEnded;

	/** Called Right before beginning Dungeon Phase */
	UPROPERTY(BlueprintAssignable)
	FDungeonPhaseBegin OnDungeonPhaseBegin;

	/** Meshes used in Online Play */
	UPROPERTY(EditDefaultsOnly)
	TArray<USkeletalMesh*> Meshes;

	
protected:
	virtual void BeginPlay() override;

	
	
private:

	
};
