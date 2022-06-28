// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ArenaPlayerState.generated.h"

struct FPlayerSpellSlot;
/**
 * @brief contains inventory copy to allow state persistence across maps
 */
UCLASS()
class ARENAGAME_API AArenaPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	/* updates the player index used to */
	void UpdatePlayerIndex(uint8 NewIndex) { PlayerIndex=NewIndex; OnRep_PlayerIndex(); }

	/** retrieves the player index used for setting the player mesh */
	uint8 GetIndex() const { return PlayerIndex; }

	/** retrieves the inventory after a server travel to keep in sync */
	void RetrieveInventory(TArray<FPlayerSpellSlot>& OutSpells, TArray<UObject*>& OutItems);

	/** sets the inventory to be up to date before travelling to new map */
	void SetInventory(TArray<FPlayerSpellSlot> InSpells, TArray<UObject*> InItems);

	/** for safety, copies over properties when changing maps */
	virtual void CopyProperties(APlayerState* PlayerState) override;

	/** helper to avoid doubly replacing inventory when playing with multiple players, only syncing once */
	bool ShouldReplaceInventory() const { return bShouldReplace; }
protected:

	UPROPERTY(Replicated, ReplicatedUsing=OnRep_PlayerIndex)
	uint8 PlayerIndex;

	UFUNCTION()
	void OnRep_PlayerIndex();

	FTimerHandle RetryHandle;

	UPROPERTY(Replicated)
	TArray<FPlayerSpellSlot> SpellSlots;

	UPROPERTY(Replicated)
	TArray<UObject*> Items;

	UPROPERTY(Replicated)
	bool bShouldReplace=false;
	
};
