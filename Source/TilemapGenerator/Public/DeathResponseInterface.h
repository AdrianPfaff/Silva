// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DeathResponseInterface.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDeath, ACharacter*, DeadCharacter);

// This class does not need to be modified.
UINTERFACE()
class UDeathResponseInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TILEMAPGEN_API IDeathResponseInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	FOnEnemyDeath OnDeath;
};
