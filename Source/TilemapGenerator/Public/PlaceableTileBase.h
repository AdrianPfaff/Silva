// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "TileBase.h"
#include "PlaceableTileBase.generated.h"

/**
 * Intermediate inheritance to make it easier to differentiate between actual gameplay spawned tiles and special ones
 */
UCLASS(Abstract, Blueprintable)
class TILEMAPGEN_API APlaceableTileBase : public ATileBase
{
	GENERATED_BODY()
	
};
