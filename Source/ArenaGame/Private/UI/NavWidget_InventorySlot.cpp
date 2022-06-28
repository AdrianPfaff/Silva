// Copyright Runescripters 2021


#include "UI/NavWidget_InventorySlot.h"

#include "Components/Image.h"
#include "Core/ArenaPlayerCharacter.h"
#include "UI/InventoryItemWrapper.h"
#include "UI/NavWidget_Inventory.h"
#include "UI/NavWidget_InventoryRuneSlot.h"
#include "UI/NavWidget_InventorySpellSlot.h"


void UNavWidget_InventorySlot::UpdateInfoUI()
{
	if (!ensure(OwningInventory)) return;

	//Dont do anything if no item is present
	if (!ContainsItem() || !PeekItem()->ContainsItem())
	{
		return;
	}
	OwningInventory->UpdateItemInfo(PeekItem()->GetDisplayName(), PeekItem()->GetDescription());
}

bool UNavWidget_InventorySlot::Initialize()
{
	bool bSuccess = Super::Initialize();

	Icon->SetBrushFromTexture(EmptyIcon, true);
	
	return bSuccess;
}

UNavWidget_InventorySlot* UNavWidget_InventorySlot::SwapSlots(UNavWidget_InventorySlot* OtherSlot)
{
	if (!OtherSlot) return OtherSlot;
	/* Possible cases:
	 *	Inventory to Inventory
	 *	Rune to Inventory
	 *	Spell to Inventory
	 */

	if (auto RuneSlot=Cast<UNavWidget_InventoryRuneSlot>(OtherSlot))
	{
		return SwapFromRune(RuneSlot);
	}

	if (auto SpellSlot=Cast<UNavWidget_InventorySpellSlot>(OtherSlot))
	{
		return SwapFromSpell(SpellSlot);
	}

	return SwapFromInventory(OtherSlot);
}

void UNavWidget_InventorySlot::ClearSlot()
{
	SlotItem=nullptr;
	Icon->SetBrushFromTexture(EmptyIcon, true);
	Icon->SetOpacity(1.f);
}

bool UNavWidget_InventorySlot::ContainsItem() const
{
	return SlotItem!=nullptr;
}

void UNavWidget_InventorySlot::PutItem(UInventoryItemWrapper* Item)
{
	if (!Item) return;
	
	SlotItem=Item;
	if (SlotItem->ContainsItem())
	{
		Icon->SetBrushFromTexture(SlotItem->GetIcon(), true);
		Icon->SetOpacity(1.f);
	}
	else
	{
		Icon->SetBrushFromTexture(EmptyIcon, true);
		Icon->SetOpacity(1.f);
	}
	
}

UInventoryItemWrapper* UNavWidget_InventorySlot::RetrieveItem()
{
	UInventoryItemWrapper* RetItem=SlotItem;
	SlotItem=nullptr;
	Icon->SetBrushFromTexture(EmptyIcon, true);
	Icon->SetOpacity(1.f);
	return RetItem;
}

const UInventoryItemWrapper* UNavWidget_InventorySlot::PeekItem() const
{
	return SlotItem;
}

void UNavWidget_InventorySlot::SwapSlotsUnconditional_Internal(UNavWidget_InventorySlot* OtherSlot)
{
	auto OtherItem=OtherSlot->RetrieveItem();
	auto ThisItem=RetrieveItem();
	OtherSlot->PutItem(ThisItem);
	PutItem(OtherItem);
}

UNavWidget_InventorySlot* UNavWidget_InventorySlot::SwapFromRune(UNavWidget_InventoryRuneSlot* OtherSlot)
{
	//Only Swap if Item Types are identical
	URuneBase* Rune=OtherSlot->PeekItem() ? OtherSlot->PeekItem()->GetRune() : nullptr;
	bool bIsSpell=PeekItem() ? PeekItem()->GetItemType()==EItemType::Spell : false;
	if (bIsSpell)
	{
		OtherSlot->Icon->SetOpacity(1.f);
		Icon->SetOpacity(1.f);
		return OtherSlot;
	}

	auto PlayerInventory=Cast<AArenaPlayerCharacter>(GetOwningPlayerPawn())->GetInventoryComponent();
	PlayerInventory->SetRuneInSpell(OtherSlot->GetInputID(), PeekItem() ? PeekItem()->GetRune() : nullptr, OtherSlot->GetRuneIndex());
	UObject* OldItem=nullptr;
	PlayerInventory->ReplaceItemAtIndex(Rune, GetInventoryIndex(), OldItem);

	//Swap items in UI
	SwapSlotsUnconditional_Internal(OtherSlot);
	return OtherSlot;
	
	
}

UNavWidget_InventorySlot* UNavWidget_InventorySlot::SwapFromInventory(UNavWidget_InventorySlot* OtherSlot)
{
	//Swap items in UI
	SwapSlotsUnconditional_Internal(OtherSlot);

	UObject* Item=PeekItem() ? (PeekItem()->GetItemType()==EItemType::Rune ? Cast<UObject>(PeekItem()->GetRune()) : Cast<UObject>(PeekItem()->GetSpell())) : nullptr;
	UObject* OtherItem=OtherSlot->PeekItem() ? (OtherSlot->PeekItem()->GetItemType()==EItemType::Rune ? Cast<UObject>(OtherSlot->PeekItem()->GetRune()) : Cast<UObject>(OtherSlot->PeekItem()->GetSpell())) : nullptr;

	auto PlayerInventory=Cast<AArenaPlayerCharacter>(GetOwningPlayerPawn())->GetInventoryComponent();
	
	PlayerInventory->SwapIndices(GetInventoryIndex(), OtherSlot->GetInventoryIndex());
	return OtherSlot;
	
}

UNavWidget_InventorySlot* UNavWidget_InventorySlot::SwapFromSpell(UNavWidget_InventorySpellSlot* OtherSlot)
{
	//Only swap if Item Types are identical
	UGASAbilitySpell* Spell=OtherSlot->PeekItem() ? OtherSlot->PeekItem()->GetSpell() : nullptr;
	bool bIsRune=PeekItem() ? PeekItem()->GetItemType()==EItemType::Rune : false;
	if (bIsRune)
	{
		OtherSlot->Icon->SetOpacity(1.f);
		Icon->SetOpacity(1.f);
		return OtherSlot;
	}
	
	//Equip new spell, overriding old one
	auto Player=Cast<AArenaPlayerCharacter>(GetOwningPlayerPawn());
	UGASAbilitySpell* ThisSpell=PeekItem() ? PeekItem()->GetSpell() : nullptr;
	Player->GrantNewAbilityInSlot(ThisSpell ? ThisSpell->GetClass() : nullptr, OtherSlot->GetInputID());
	UObject* OldItem=nullptr;
	Player->GetInventoryComponent()->ReplaceItemAtIndex(OtherSlot->PeekItem() ? OtherSlot->PeekItem()->GetSpell() : nullptr, GetInventoryIndex(), OldItem);
	//Swap ui
	SwapSlotsUnconditional_Internal(OtherSlot);

	return OtherSlot;
	
}
