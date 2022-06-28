// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "DA_EnemySpawnGroup.h"
#include "SpawnVolume.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TILEMAPGEN_API USpawnVolume : public UBoxComponent
{
	GENERATED_BODY()
public:
	
	USpawnVolume();
	
};
