// Copyright Runescripters 2021


#include "UI/InventoryItemWrapper.h"

#include "GAS/GASAbilitySpell.h"
#include "GAS/RuneBase.h"

URuneBase* UInventoryItemWrapper::GetRune() const
{
	if (!bHasBeenSetup)
	{
		UE_LOG(LogSilvaUI, Error, TEXT("Iventory Item Wrapper has not been setup correctly"))
		return nullptr;
	}
	return Rune;
}

UGASAbilitySpell* UInventoryItemWrapper::GetSpell() const
{
	if (!bHasBeenSetup)
	{
		UE_LOG(LogSilvaUI, Error, TEXT("Iventory Item Wrapper has not been setup correctly"))
		return nullptr;
	}
	return Spell;
}

void UInventoryItemWrapper::Setup(URuneBase* InRune)
{
	Rune=InRune;
	bHasBeenSetup=true;
}

void UInventoryItemWrapper::Setup(UGASAbilitySpell* InSpell)
{
	Spell=InSpell;
	bHasBeenSetup=true;
}

UTexture2D* UInventoryItemWrapper::GetIcon() const
{
	if (!bHasBeenSetup)
	{
		UE_LOG(LogSilvaUI, Error, TEXT("Iventory Item Wrapper has not been setup correctly"))
		return nullptr;
	}
	return Rune? Rune->GetIcon() : Spell->GetIcon();
}

const FText& UInventoryItemWrapper::GetDisplayName() const
{
	if (!bHasBeenSetup)
	{
		UE_LOG(LogSilvaUI, Error, TEXT("Iventory Item Wrapper has not been setup correctly"))
		return FText::GetEmpty();
	}
	return Rune? Rune->GetDisplayName() : Spell->GetDisplayName();
}

const FText& UInventoryItemWrapper::GetDescription() const
{
	if (!bHasBeenSetup)
	{
		UE_LOG(LogSilvaUI, Error, TEXT("Iventory Item Wrapper has not been setup correctly"))
		return FText::GetEmpty();
	}
	return Rune? Rune->GetDescription() : Spell->GetDescription();
}
