// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "NavWidget_InventorySlot.h"
#include "Blueprint/UserWidget.h"
#include "RemoveItemBackdrop.generated.h"

class APickup;

/**
 * Used in the inventory to allow the player to drop an item by dragging it outside of the inventory.
 */
UCLASS()
class ARENAGAME_API URemoveItemBackdrop : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void DropItem(UInventoryItemWrapper* Item, UNavWidget_InventorySlot* OriginSlot);

protected:

	UPROPERTY(EditDefaultsOnly, meta=(AllowedClasses=Pickup))
	TSubclassOf<AActor> PickupClass;
	
};
