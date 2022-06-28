// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "ArenaGame/ArenaGame.h"
#include "Components/ActorComponent.h"
#include "PlayerInventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FGoldChanged, float)

class UGASAbilitySpell;
class URuneBase;

USTRUCT(BlueprintType)
struct FPlayerSpellSlot
{
	GENERATED_BODY()
	FPlayerSpellSlot() : AbilityHandle(FGameplayAbilitySpecHandle()), AbilitySpell(nullptr)
	{
		Runes.Add(nullptr);
		Runes.Add(nullptr);
		Runes.Add(nullptr);
	}
	
	FGameplayAbilitySpecHandle AbilityHandle;

	UPROPERTY()
	UGASAbilitySpell* AbilitySpell;
	
	UPROPERTY()
	TArray<URuneBase*> Runes;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENAGAME_API UPlayerInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerInventoryComponent();

	

	UFUNCTION(Server, Reliable)
	void SetSpellInSlot(EGASAbilityInputID InputID, FGameplayAbilitySpecHandle Handle, UGASAbilitySpell* Spell);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="GAS")
	void SetRuneInSpell(EGASAbilityInputID InInputID, URuneBase* NewRune, uint8 RuneSlot);

	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	
	/** Tries to add an Item to the inventory. Doesnt do anything if full */
	void AddItem(UObject* Item);

	UFUNCTION(Server, Reliable)
	/** Tries to remove an Item from the inventory. Doesnt do anything if not contained */
	void RemoveItem(UObject* Item);

	UFUNCTION(Server, Reliable)
	/** Swaps item internally to keep representation in line with UI */
	void SwapIndices(int32 A, int32 B);

	UFUNCTION(Server, Reliable)
	void ReplaceItemAtIndex(UObject* InNewItem, int32 Index, UObject* OutOldItem);

	/** Checks if Inventory has free slots */
	UFUNCTION(BlueprintPure)
	bool HasFreeSlots() const;

	UFUNCTION(BlueprintPure)
	const TArray<FPlayerSpellSlot>& GetSpellSlots() const;
	
	UFUNCTION(BlueprintPure)
	const TArray<UObject*>& GetItems() const;

	/** Used to transfer inventory after travelling to new map */
	void ReplaceInventory(TArray<FPlayerSpellSlot>& InSlots, TArray<UObject*>& InItems);

	/** Used to transfer inventory after travelling to new map */
	void CopyInventory(TArray<FPlayerSpellSlot>& OutSlots, TArray<UObject*>& OutItems);

	UFUNCTION(BlueprintCallable)
	bool PurchaseItem(TSubclassOf<UObject> ItemToBuy, int32 Price);

	UFUNCTION(BlueprintCallable)
	bool PurchasePotion(int32 Price);
	
	void PickupGold(int32 Amount);

	FGoldChanged OnGoldChanged;

	UFUNCTION()
	void OnRep_GoldChanged();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	/** Amount of Slots this Inventory contains */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxItems=9;

	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated)
	TArray<FPlayerSpellSlot> SpellSlots;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	TArray<UObject*> Items;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, ReplicatedUsing=OnRep_GoldChanged)
	int32 Gold = 0;
private:
	UFUNCTION(Server, Reliable)
	void PurchasePotionServer(int32 Price);
	
	UFUNCTION(Server, Reliable)
	void PurchaseItemServer(TSubclassOf<UObject> ItemToBuy, int32 Price);

	void UpdateGoldValue(float Difference);
	
};
