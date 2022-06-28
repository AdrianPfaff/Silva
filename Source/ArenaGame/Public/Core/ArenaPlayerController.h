// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "PlayerInventoryComponent.h"
#include "GameFramework/PlayerController.h"
#include "UI/NavWidget_HUD.h"
#include "UI/NavWidget_Inventory.h"
#include "ArenaPlayerController.generated.h"

/**
 * @brief The controller used by all players
 */
UCLASS()
class ARENAGAME_API AArenaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AArenaPlayerController();

	/** Updates the Spell Icons form Server */
	UFUNCTION(Client, Reliable)
	void UpdateSpellIcons();
	
	virtual void OnPossess(APawn* InPawn) override;

	/** Toggles the HUD from the Server */
	UFUNCTION(Client, Reliable)
	void ToggleHUD(bool bVisible);

	/** Updates the Spell Icons form Server */
	virtual void GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList) override;
	
	/** Updates UI Cooldown from Server */
	UFUNCTION(Client, Reliable)
	void SetAbilityCooldownInUI(EGASAbilityInputID InputID, float Cooldown);

	/** setups UI Bindings on client */
	virtual void BeginPlay() override;

	virtual void PostSeamlessTravel() override;
	
	/** Before travelling to new map, copy over inventory state and remove hud */
	virtual void PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel) override;

	/** Setups UI */
	void SetupUI();
	
	

	UPROPERTY()
	bool bAfterSeamlessTravel=false;

	/** creates a timer in the UI, overriding the old one if present*/
	UFUNCTION(Client, Reliable)
	void SetupTimerUI(const FTimespan& InTime);

	/** Displays a message popup */
	UFUNCTION(Client, Reliable)
	void DisplayMsg(const FString& Msg);

	/** prepares inventory before travelling to new map */
	void PrepareInventoryForTravel();

protected:

	/** opens inventory */
	UFUNCTION()
	void OpenInventory();

	/** sets up toggle for opening inventory */
	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UNavWidget_HUD> HUDClass;

	UPROPERTY(BlueprintReadOnly)
	UNavWidget_HUD* HUD;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UNavWidget_Inventory> InventoryClass;

	UPROPERTY(BlueprintReadOnly)
	UNavWidget_Inventory* Inventory;
	
	
};
