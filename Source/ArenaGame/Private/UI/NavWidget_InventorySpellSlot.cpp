// Copyright Runescripters 2021


#include "UI/NavWidget_InventorySpellSlot.h"

#include "Components/Image.h"
#include "Core/ArenaPlayerCharacter.h"
#include "UI/InventoryItemWrapper.h"
#include "UI/NavWidget_InventoryRuneSlot.h"


UNavWidget_InventorySlot* UNavWidget_InventorySpellSlot::SwapSlots(UNavWidget_InventorySlot* OtherSlot)
{
	if(!OtherSlot) return OtherSlot;
	/* Possible cases:
	 * Swap from Rune->Abort
	 * Swap from other Spell
	 * Swap From Inventory->Abort if Rune, allow if Spell
	 */

	if (auto RuneSlot=Cast<UNavWidget_InventoryRuneSlot>(OtherSlot))
	{
		OtherSlot->Icon->SetOpacity(1.f);
		return OtherSlot;
	}

	if (auto SpellSlot=Cast<UNavWidget_InventorySpellSlot>(OtherSlot))
	{
		return SwapFromSpell(SpellSlot);
	}

	return SwapFromInventory(OtherSlot);
}

UNavWidget_InventorySlot* UNavWidget_InventorySpellSlot::SwapFromSpell(UNavWidget_InventorySpellSlot* OtherSlot)
{
	SwapSlotsUnconditional_Internal(OtherSlot);
	OtherSlot->RefreshSpell();
	RefreshSpell();
	return OtherSlot;
}

UNavWidget_InventorySlot* UNavWidget_InventorySpellSlot::SwapFromInventory(UNavWidget_InventorySlot* OtherSlot)
{
	OtherSlot->Icon->SetOpacity(1.f);
	if (OtherSlot->PeekItem() ? OtherSlot->PeekItem()->GetItemType()!=EItemType::Spell : false) return OtherSlot;
	
	UGASAbilitySpell* Spell=OtherSlot->PeekItem() ? OtherSlot->PeekItem()->GetSpell() : nullptr;
	auto PlayerCharacter=Cast<AArenaPlayerCharacter>(GetOwningPlayerPawn());

	UGASAbilitySpell* NewSpell=nullptr;
	PlayerCharacter->GetInventoryComponent()->ReplaceItemAtIndex(PeekItem() ? PeekItem()->GetSpell() : nullptr, OtherSlot->GetInventoryIndex(), NewSpell);
	SwapSlotsUnconditional_Internal(OtherSlot);
	RefreshSpell();
	return OtherSlot;
}

void UNavWidget_InventorySpellSlot::RefreshSpell()
{
	auto PlayerCharacter=Cast<AArenaPlayerCharacter>(GetOwningPlayerPawn());
	if (!PlayerCharacter)
	{
		UE_LOG(LogSilvaUI, Error, TEXT("Tried to equip spell on an actor that is not an AArenaPlayerCharacter"))
		return;
	}
	if (SlotItem)
	{
		PlayerCharacter->GrantNewAbilityInSlot(SlotItem->GetSpell() ? SlotItem->GetSpell()->GetClass() : nullptr, InputID);
	}
	else
	{
		PlayerCharacter->GrantNewAbilityInSlot(nullptr, InputID);
	}
	
}
