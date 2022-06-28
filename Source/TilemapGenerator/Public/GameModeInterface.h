// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "EnumLibrary.h"
#include "UObject/Interface.h"
#include "GameModeInterface.generated.h"

class AChest;
enum class EEnemyDifficulty : uint8;
class UDA_EnemySpawnGroup;
// This class does not need to be modified.
UINTERFACE()
class TILEMAPGEN_API UGameModeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TILEMAPGEN_API IGameModeInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual UDA_EnemySpawnGroup* GetSpawnGroup(ETileType TileType) { return nullptr; }

	virtual UClass* GetChestClass(ETileType Difficulty) { return nullptr; }

	virtual void HandlePlayerDeath(ACharacter* DeadPlayer, float& OutRespawnDelay) {};
};


