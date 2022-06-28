// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "ArenaGame/ArenaGame.h"
#include "UI/NavWidget_InventorySlot.h"
#include "NavWidget_InventorySpellSlot.generated.h"

/**
 * Widget modeling a specialized slot in the Player Inventory to equip spells
 */
UCLASS()
class ARENAGAME_API UNavWidget_InventorySpellSlot : public UNavWidget_InventorySlot
{
	GENERATED_BODY()

public:
	void RefreshSpell();
	
	virtual UNavWidget_InventorySlot* SwapSlots(UNavWidget_InventorySlot* OtherSlot) override;

	EGASAbilityInputID GetInputID() { return InputID; }


protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	EGASAbilityInputID InputID;

private:

	UNavWidget_InventorySlot* SwapFromSpell(UNavWidget_InventorySpellSlot* OtherSlot);
	UNavWidget_InventorySlot* SwapFromInventory(UNavWidget_InventorySlot* OtherSlot);
};
