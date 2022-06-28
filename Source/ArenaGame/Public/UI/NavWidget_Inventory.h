// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "DA_InventoryItemInfo.h"
#include "UINavWidget.h"
#include "NavWidget_Inventory.generated.h"

class UTextBlock;
class UNavWidget_InventorySlot;
class UNavWidget_InventorySpellSlot;
class UNavWidget_InventoryRuneSlot;
/**
 * Widget for the Player Inventory
 */
UCLASS()
class ARENAGAME_API UNavWidget_Inventory : public UUINavWidget
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;

	void UpdateItemInfo(const FText& DisplayName, const FText& Description);

protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UTextBlock* ItemName;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UTextBlock* ItemDescription;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UNavWidget_InventorySlot* InventorySlot0;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UNavWidget_InventorySlot* InventorySlot1;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UNavWidget_InventorySlot* InventorySlot2;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UNavWidget_InventorySlot* InventorySlot3;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UNavWidget_InventorySlot* InventorySlot4;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UNavWidget_InventorySlot* InventorySlot5;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UNavWidget_InventorySlot* InventorySlot6;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UNavWidget_InventorySlot* InventorySlot7;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UNavWidget_InventorySlot* InventorySlot8;

	UPROPERTY(BlueprintReadOnly)
	TArray<UNavWidget_InventorySlot*> InventorySlotsArray;

	UPROPERTY(BlueprintReadOnly)
	TArray<UNavWidget_InventorySpellSlot*> SpellSlotsArray;

	
	TArray<TArray<UNavWidget_InventoryRuneSlot*>> RuneSlots;

	TArray<UNavWidget_InventoryRuneSlot*> RuneSlots0;
	TArray<UNavWidget_InventoryRuneSlot*> RuneSlots1;
	TArray<UNavWidget_InventoryRuneSlot*> RuneSlots2;

	UPROPERTY(BlueprintReadOnly, meta=(BindWiget))
	UNavWidget_InventorySpellSlot* InventorySpellSlot0;

	UPROPERTY(BlueprintReadOnly, meta=(BindWiget))
	UNavWidget_InventorySpellSlot* InventorySpellSlot1;

	UPROPERTY(BlueprintReadOnly, meta=(BindWiget))
	UNavWidget_InventorySpellSlot* InventorySpellSlot2;

	UPROPERTY(BlueprintReadOnly, meta=(BindWiget))
	UNavWidget_InventoryRuneSlot* InventoryRuneSlot00;

	UPROPERTY(BlueprintReadOnly, meta=(BindWiget))
	UNavWidget_InventoryRuneSlot* InventoryRuneSlot01;

	UPROPERTY(BlueprintReadOnly, meta=(BindWiget))
	UNavWidget_InventoryRuneSlot* InventoryRuneSlot02;

	UPROPERTY(BlueprintReadOnly, meta=(BindWiget))
	UNavWidget_InventoryRuneSlot* InventoryRuneSlot10;

	UPROPERTY(BlueprintReadOnly, meta=(BindWiget))
	UNavWidget_InventoryRuneSlot* InventoryRuneSlot11;

	UPROPERTY(BlueprintReadOnly, meta=(BindWiget))
	UNavWidget_InventoryRuneSlot* InventoryRuneSlot12;

	UPROPERTY(BlueprintReadOnly, meta=(BindWiget))
	UNavWidget_InventoryRuneSlot* InventoryRuneSlot20;

	UPROPERTY(BlueprintReadOnly, meta=(BindWiget))
	UNavWidget_InventoryRuneSlot* InventoryRuneSlot21;

	UPROPERTY(BlueprintReadOnly, meta=(BindWiget))
	UNavWidget_InventoryRuneSlot* InventoryRuneSlot22;

private:
	void ClearAllSlots();
};
