// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "InventoryItemWrapper.generated.h"

class URuneBase;
class UGASAbilitySpell;

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Spell=0,
	Rune=1
};
/**
 * Wrapper class for the visual representation of Items in the Inventory. Mostly forwards to actual contained item, but
 * also has knowledge about the type contained.
 */
UCLASS(BlueprintType)
class ARENAGAME_API UInventoryItemWrapper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	/** Returns the type of the Item */
	EItemType GetItemType() const { return Spell ? EItemType::Spell : EItemType::Rune; }

	UFUNCTION(BlueprintPure)
	/** Returns a pointer to the Rune. Returns nullptr if invalid */
	URuneBase* GetRune() const;
	UFUNCTION(BlueprintPure)
	/** Returns a pointer to the Spell class. Returns nullptr if invalid */
	UGASAbilitySpell* GetSpell() const;

	void Setup(URuneBase* InRune);
	void Setup(UGASAbilitySpell* InSpell);

	UFUNCTION(BlueprintPure)
	UTexture2D* GetIcon() const;

	UFUNCTION(BlueprintPure)
	const FText& GetDisplayName() const;

	UFUNCTION(BlueprintPure)
	const FText& GetDescription() const;

	UFUNCTION(BlueprintPure)
	bool ContainsItem() const {
		return Rune||Spell ? true : false;
	}

	
private:
	UPROPERTY()
	URuneBase* Rune;

	UPROPERTY()
	UGASAbilitySpell* Spell;

	bool bHasBeenSetup=false;
	
	
};
