// Copyright Runescripters 2021

#pragma once

#include "SubclassTableRow.h"
#include "EnumLibrary.h"
#include "TableRows.generated.h"

USTRUCT(BlueprintType)
struct TILEMAPGEN_API FEnemiesTableRow : public FSubclassTableRow
{
	GENERATED_USTRUCT_BODY()

	FEnemiesTableRow() : Difficulty(EEnemyDifficulty::Normal) {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Table Row")
	EEnemyDifficulty Difficulty;
};
	
