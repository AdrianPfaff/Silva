// Copyright Runescripters 2021


#include "UI/NavWidget_InventoryRuneSlot.h"

#include "Components/Image.h"
#include "Core/ArenaPlayerCharacter.h"
#include "UI/InventoryItemWrapper.h"
#include "UI/NavWidget_InventorySpellSlot.h"


UNavWidget_InventorySlot* UNavWidget_InventoryRuneSlot::SwapSlots(UNavWidget_InventorySlot* OtherSlot)
{
	if (!OtherSlot) return OtherSlot;
	/* Possible cases:
	 * Swap from other Spell->Abort
	 * Swap from other Rune Slot
	 * Swap From Inventory->Abort if Spell, allow if Rune
	 */

	if (auto OtherSpell=Cast<UNavWidget_InventorySpellSlot>(OtherSlot))
	{
		OtherSlot->Icon->SetOpacity(1.f);
		return OtherSlot;
	}

	if (auto OtherRune=Cast<UNavWidget_InventoryRuneSlot>(OtherSlot))
	{
		return SwapFromRune(OtherRune);
	}

	return SwapFromInventory(OtherSlot);
}

void UNavWidget_InventoryRuneSlot::RefreshRune()
{
	auto PlayerCharacter=Cast<AArenaPlayerCharacter>(GetOwningPlayerPawn());
	PlayerCharacter->GetInventoryComponent()->SetRuneInSpell(InputID, SlotItem ? SlotItem->GetRune() : nullptr, RuneIndex);
}

UNavWidget_InventorySlot* UNavWidget_InventoryRuneSlot::SwapFromRune(UNavWidget_InventoryRuneSlot* OtherSlot)
{
	OtherSlot->Icon->SetOpacity(1.f);
	SwapSlotsUnconditional_Internal(OtherSlot);
	OtherSlot->RefreshRune();
	RefreshRune();
	return OtherSlot;
}

UNavWidget_InventorySlot* UNavWidget_InventoryRuneSlot::SwapFromInventory(UNavWidget_InventorySlot* OtherSlot)
{
	OtherSlot->Icon->SetOpacity(1.f);
	if (OtherSlot->PeekItem() ? OtherSlot->PeekItem()->GetItemType()!=EItemType::Rune : false) return OtherSlot;

	URuneBase* Rune=OtherSlot->PeekItem() ? OtherSlot->PeekItem()->GetRune() : nullptr;
	auto PlayerCharacter=Cast<AArenaPlayerCharacter>(GetOwningPlayerPawn());

	URuneBase* NewRune=nullptr;
	PlayerCharacter->GetInventoryComponent()->ReplaceItemAtIndex(PeekItem() ? PeekItem()->GetRune() : nullptr, OtherSlot->GetInventoryIndex(), NewRune);
	SwapSlotsUnconditional_Internal(OtherSlot);
	RefreshRune();
	return OtherSlot;
}
