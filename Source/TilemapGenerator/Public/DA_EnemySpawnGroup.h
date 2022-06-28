// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "EnumLibrary.h"
#include "Engine/DataAsset.h"
#include "DA_EnemySpawnGroup.generated.h"

UENUM(BlueprintType)
enum class EEnemySpawnType : uint8
{
	Near UMETA(Tooltip = "Spawn close to the player"),
	Far UMETA(Tooltip = "Spawn far away from the player")
};

USTRUCT(BlueprintType)
struct FEnemyBundle
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEnemySpawnType SpawnType=EEnemySpawnType::Near;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSoftClassPtr<ACharacter>> Enemies;
};

/**
 * 
 */
UCLASS(BlueprintType)
class TILEMAPGEN_API UDA_EnemySpawnGroup : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEnemyDifficulty Difficulty=EEnemyDifficulty::Normal;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(TitleProperty="SpawnType"))
	TArray<FEnemyBundle> EnemyBundles;
	
};
