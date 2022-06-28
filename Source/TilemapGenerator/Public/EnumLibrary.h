// Copyright Runescripters 2021

#pragma once

#include "EnumLibrary.generated.h"

/**
 * The direction enum will be used throughout the plugin to make handling physical directions easier. Also helps with
 * storing things in TArrays because they can easily be converted to the index equal to the direction.
 * It is defined as followed
 * North: +X
 * East: +Y
 * South: -X
 * West: -Y
 */
UENUM(BlueprintType)
enum class EDirection : uint8
{
	North=0 UMETA(DisplayName = "North", ToolTip="+X Axis"),
	East=1 UMETA(DisplayName = "East", ToolTip="+Y Axis"),
	South=2 UMETA(DisplayName = "South", ToolTip="-X Axis"),
	West=3 UMETA(DisplayName = "West", ToolTip="-Y Axis")
	
};
ENUM_RANGE_BY_FIRST_AND_LAST(EDirection, EDirection::North, EDirection::West)


UENUM(BlueprintType)
enum class ETileType : uint8
{
	Start=0 UMETA(DisplayName= "Start", ToolTip="Tile where the players will start"),
	Shop=1 UMETA(DisplayName= "Shop"),
	Easy=2 UMETA(DisplayName = "Easy", ToolTip="Normal Tile, Easy Difficulty"),
	Normal=3 UMETA(DisplayName = "Normal", ToolTip="Normal Tile, Normal Difficulty"),
	Hard=4 UMETA(DisplayName = "Hard", ToolTip="Normal Tile, Hard Difficulty")
};

UENUM(BlueprintType)
enum class EEnemyDifficulty : uint8
{
	Easy=0 UMETA(DisplayName = "Easy"),
	Normal=1 UMETA(DisplayName = "Normal"),
	Hard=2 UMETA(DisplayName = "Hard")
};