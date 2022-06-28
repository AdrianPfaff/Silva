// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "GAS/GASAbilitySpell.h"
#include "GASRuneSpell.generated.h"

/**
 * 
 */
UCLASS()
class ARENAGAME_API UGASRuneSpell : public UGASAbilitySpell
{
	GENERATED_BODY()

public:

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	
};
