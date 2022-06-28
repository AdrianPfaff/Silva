// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "UI/NavWidget_InventorySlot.h"
#include "ArenaGame/ArenaGame.h"
#include "NavWidget_InventoryRuneSlot.generated.h"

/**
 * Widget modeling a specialized slot in the Player Inventory to equip runes
 */
UCLASS()
class ARENAGAME_API UNavWidget_InventoryRuneSlot : public UNavWidget_InventorySlot
{
	GENERATED_BODY()

public:

	virtual UNavWidget_InventorySlot* SwapSlots(UNavWidget_InventorySlot* OtherSlot) override;

	void RefreshRune();

	uint8 GetRuneIndex() const { return RuneIndex; }
	EGASAbilityInputID GetInputID() const { return InputID; }

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	uint8 RuneIndex;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	EGASAbilityInputID InputID;

private:

	UNavWidget_InventorySlot* SwapFromRune(UNavWidget_InventoryRuneSlot* OtherSlot);
	UNavWidget_InventorySlot* SwapFromInventory(UNavWidget_InventorySlot* OtherSlot);
};
