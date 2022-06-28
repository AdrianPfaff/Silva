// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "UINavWidget.h"
#include "NavWidget_InventorySlot.generated.h"

class UInventoryItemWrapper;
class UNavWidget_Inventory;
class UImage;

/**
 * Widget modeling a generalized slot in the Player Inventory to store picked up runes and spells. Contains data to
 * the actual item using the ItemWrapper Class.
 */
UCLASS()
class ARENAGAME_API UNavWidget_InventorySlot : public UUINavWidget
{
	GENERATED_BODY()

public:

	/** Function to update the displayed item info */
	UFUNCTION(BlueprintCallable)
	void UpdateInfoUI();

	/** Initializes this slot with an default empty icon */
	virtual bool Initialize() override;

	/** Swaps content of this slot with the content of the other slot, performing necessary checks */
	UFUNCTION(BlueprintCallable)
	virtual UNavWidget_InventorySlot* SwapSlots(UNavWidget_InventorySlot* OtherSlot);
	
	/** Clears Slot, discards contained item */
	UFUNCTION(BlueprintCallable)
	void ClearSlot();

	/** Checks if an item is contained */
	UFUNCTION(BlueprintPure)
	bool ContainsItem() const;

	/** Replaces the item in the slot, not checking if any is contained */
	UFUNCTION(BlueprintCallable)
	void PutItem(UInventoryItemWrapper* Item);

	/** Gets Item and removes it from the slot */
	UFUNCTION(BlueprintCallable)
	UInventoryItemWrapper* RetrieveItem();

	/** Gets a ref to the item but does not remove it */
	UFUNCTION(BlueprintPure)
	const UInventoryItemWrapper* PeekItem() const;
	
	/** Widget Binding for the Icon Widget */
	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	UImage* Icon;

	int32 GetInventoryIndex() const { return InvSlotIndex; }

	void SetOwningInventory(UNavWidget_Inventory* Inv) { OwningInventory=Inv; }
	
	protected:

	UPROPERTY(BlueprintReadOnly)
	UNavWidget_Inventory* OwningInventory;
	
	/** The contained item */
	UPROPERTY()
	UInventoryItemWrapper* SlotItem;

	//** Icon to show when Slot is empty */
	UPROPERTY(EditDefaultsOnly)
	UTexture2D* EmptyIcon;

	UPROPERTY(EditInstanceOnly)
	int32 InvSlotIndex=-1;

	/** The internal function to swap the slots unconditionally, checks must be performed beforehand */
	void SwapSlotsUnconditional_Internal(UNavWidget_InventorySlot* OtherSlot);
private:
	
	UNavWidget_InventorySlot* SwapFromRune(class UNavWidget_InventoryRuneSlot* OtherSlot);

	UNavWidget_InventorySlot* SwapFromSpell(class UNavWidget_InventorySpellSlot* OtherSlot);

	UNavWidget_InventorySlot* SwapFromInventory(UNavWidget_InventorySlot* OtherSlot);
	
};
