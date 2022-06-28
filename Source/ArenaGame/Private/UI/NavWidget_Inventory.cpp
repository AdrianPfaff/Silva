// Copyright Runescripters 2021


#include "UI/NavWidget_Inventory.h"

#include "Components/TextBlock.h"
#include "Core/ArenaPlayerCharacter.h"
#include "UI/InventoryItemWrapper.h"
#include "UI/NavWidget_InventoryRuneSlot.h"
#include "UI/NavWidget_InventorySlot.h"
#include "UI/NavWidget_InventorySpellSlot.h"

void UNavWidget_Inventory::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	InventorySlotsArray.Add(InventorySlot0);
	InventorySlotsArray.Add(InventorySlot1);
	InventorySlotsArray.Add(InventorySlot2);
	InventorySlotsArray.Add(InventorySlot3);
	InventorySlotsArray.Add(InventorySlot4);
	InventorySlotsArray.Add(InventorySlot5);
	InventorySlotsArray.Add(InventorySlot6);
	InventorySlotsArray.Add(InventorySlot7);
	InventorySlotsArray.Add(InventorySlot8);

	SpellSlotsArray.Add(InventorySpellSlot0);
	SpellSlotsArray.Add(InventorySpellSlot1);
	SpellSlotsArray.Add(InventorySpellSlot2);

	RuneSlots0.Add(InventoryRuneSlot00);
	RuneSlots0.Add(InventoryRuneSlot01);
	RuneSlots0.Add(InventoryRuneSlot02);

	RuneSlots1.Add(InventoryRuneSlot10);
	RuneSlots1.Add(InventoryRuneSlot11);
	RuneSlots1.Add(InventoryRuneSlot12);

	RuneSlots2.Add(InventoryRuneSlot20);
	RuneSlots2.Add(InventoryRuneSlot21);
	RuneSlots2.Add(InventoryRuneSlot22);

	RuneSlots.Add(RuneSlots0);
	RuneSlots.Add(RuneSlots1);
	RuneSlots.Add(RuneSlots2);
	
	for	(const auto& Entry: InventorySlotsArray)
	{
		ensure(Entry);
		Entry->SetOwningInventory(this);
	}

	for (const auto& Entry : SpellSlotsArray)
	{
		ensure(Entry);
		Entry->SetOwningInventory(this);
	}
	for (const auto& EntryArray : RuneSlots)
	{
		for (const auto& Entry : EntryArray)
		{
			ensure(Entry);
			Entry->SetOwningInventory(this);
		}
	}
	
}

void UNavWidget_Inventory::NativeConstruct()
{
	Super::NativeConstruct();

	ClearAllSlots();

	auto PlayerPawn=GetOwningPlayerPawn<AArenaPlayerCharacter>();
	if (PlayerPawn)
	{
		int32 ItemSlot=0;
		for	(UObject* Item : PlayerPawn->GetInventoryComponent()->GetItems())
		{
			auto Spell=Cast<UGASAbilitySpell>(Item);
			auto Rune=Cast<URuneBase>(Item);
			if (Spell)
			{
				auto ItemWrapper=NewObject<UInventoryItemWrapper>(GetTransientPackage(), UInventoryItemWrapper::StaticClass());
				ItemWrapper->Setup(Spell);
				InventorySlotsArray[ItemSlot]->PutItem(ItemWrapper);
				++ItemSlot;
			}
			else if (Rune)
			{
				
				auto ItemWrapper=NewObject<UInventoryItemWrapper>(GetTransientPackage(), UInventoryItemWrapper::StaticClass());
				ItemWrapper->Setup(Rune);
				InventorySlotsArray[ItemSlot]->PutItem(ItemWrapper);
				++ItemSlot;
			}
			else
			{
				++ItemSlot;
			}
		}
		int32 SpellSlotIndex=0;
		for(auto& SpellSlot : PlayerPawn->GetInventoryComponent()->GetSpellSlots())
		{
			auto ItemWrapper=NewObject<UInventoryItemWrapper>(GetTransientPackage(), UInventoryItemWrapper::StaticClass());
			ItemWrapper->Setup(SpellSlot.AbilitySpell);
			SpellSlotsArray[SpellSlotIndex]->PutItem(ItemWrapper);
			int32 RuneSlotIndex=0;
			for (auto& Rune : PlayerPawn->GetInventoryComponent()->GetSpellSlots()[SpellSlotIndex].Runes)
			{
				if (!Rune)
				{
					++RuneSlotIndex;
					continue;
				}
				
				auto ItemWrapperRune=NewObject<UInventoryItemWrapper>(GetTransientPackage(), UInventoryItemWrapper::StaticClass());
				ItemWrapperRune->Setup(Rune);
				RuneSlots[SpellSlotIndex][RuneSlotIndex]->PutItem(ItemWrapperRune);

				++RuneSlotIndex;
			}
			++SpellSlotIndex;
		}
	}
}

void UNavWidget_Inventory::UpdateItemInfo(const FText& DisplayName, const FText& Description)
{
	ItemName->SetText(DisplayName);
	ItemDescription->SetText(Description);
}

void UNavWidget_Inventory::ClearAllSlots()
{
	for (auto& InvSlot: InventorySlotsArray)
	{
		InvSlot->RetrieveItem();
	}
	for (auto& SpellSlot: SpellSlotsArray)
	{
		SpellSlot->RetrieveItem();
	}
	for (auto& SlotArray: RuneSlots)
	{
		for (auto& RuneSlot: SlotArray)
		{
			RuneSlot->RetrieveItem();
		}
	}
}

