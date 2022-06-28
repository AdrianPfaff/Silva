// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerTileSetter.generated.h"

class ATileBase;
// This class does not need to be modified.
UINTERFACE()
class UPlayerTileSetter : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TILEMAPGEN_API IPlayerTileSetter
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void SetCurrentTile(ATileBase* Tile) {};
};
