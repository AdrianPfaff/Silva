// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffect.h"
#include "PlayerInventoryComponent.h"
#include "PlayerTileSetter.h"
#include "ArenaGame/ArenaGame.h"
#include "Core/ArenaCharacterBase.h"
#include "GAS/GASAbilitySpell.h"
#include "GAS/RuneBase.h"
#include "UI/NavWidget_HUD.h"
#include "ArenaPlayerCharacter.generated.h"


class ATileBase;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHealthChanged, float, CharacterHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMovementSpeedChanged, float, MovementSpeed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClampShieldVal, float, ShieldVal);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClampAppliedShield, float, AppliedShieldVal);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAbilityCommited, EGASAbilityInputID, InputID, float, Cooldown);

UENUM()
enum class EPlayerState
{
	Alive,
	Dead,
	AliveNonCombat, //cannot attack/get damaged
};
/**
 * 
 */
UCLASS()
class ARENAGAME_API AArenaPlayerCharacter : public AArenaCharacterBase, public IPlayerTileSetter
{
	GENERATED_BODY()

	public:

	AArenaPlayerCharacter();

	virtual void PossessedBy(AController* NewController) override;

	/** Updates info on which tile this player is right now */
	virtual void SetCurrentTile(ATileBase* Tile) override { OnTile=Tile; }

	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	/**
	 * @brief Grants a new ability to the player in a spell slot
	 * @param InAbility The new ability
	 * @param InInputID Slot to be used
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="GAS")
	void GrantNewAbilityInSlot(TSubclassOf<UGameplayAbility> InAbility, EGASAbilityInputID InInputID);

	void TriggerPrimarySpell() { TriggerAbilityInSlot(EGASAbilityInputID::PrimarySpell); }
	void TriggerSecondarySpell() { TriggerAbilityInSlot(EGASAbilityInputID::SecondarySpell); }
	void TriggerTertiarySpell() { TriggerAbilityInSlot(EGASAbilityInputID::TertiarySpell); }

	/** triggers an ability in a slot */
	void TriggerAbilityInSlot(EGASAbilityInputID InInputID);

	/** executes the ability triggered from client if possible */
	UFUNCTION(Server, Reliable)
	void Server_TriggerAbilityInSlot(EGASAbilityInputID InInputID);

	/** Delegate for Health Value change */
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FHealthChanged OnHealthChanged;

	/** Delegate for Shield Value change */
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FClampShieldVal ClampShieldVal;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FClampShieldVal ClampAppliedShieldVal;

	/** Delegate for Movement Speed change */
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FMovementSpeedChanged OnMovementSpeedChanged;

	/** Updates the actual movement speed from the attribute */
	UFUNCTION()
	void UpdateMovementSpeed(float NewValue);

	/** Getter for the player inventory */
	UFUNCTION(BlueprintPure)
	UPlayerInventoryComponent* GetInventoryComponent() const { return Inventory; }
	
	/** Helper to toggle friction for some abilities */
	UFUNCTION(BlueprintCallable)
	void ToggleFriction();

	/** triggers OnAbilityCommited */
	UFUNCTION()
	void NotifyAbilityCommited(FGameplayAbilitySpecHandle Handle, float Cooldown);

	/** Delegate for Ability commitment */
	UPROPERTY(BlueprintAssignable)
	FAbilityCommited OnAbilityCommited;

	/** Helper to update the skeletal mesh */
	void SetSkeletalMesh(USkeletalMesh* NewMesh);

	/** Logs info about the ability activated */
	UFUNCTION()
	void OnAbilityActivated(UGameplayAbility* GameplayAbility);
	
	UFUNCTION(BlueprintCallable)
	int GetLockoutStacks() const;

	UPROPERTY(BlueprintReadWrite);
	int PowerfluxStacks = 0;

	UFUNCTION(BlueprintCallable)
	void ResetPowerfluxStacks();
	

	UPROPERTY(BlueprintReadOnly, Replicated)
	EPlayerState PlayerStatus=EPlayerState::AliveNonCombat;
	
	protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	//Delegate Trigger for Health Attribute
    void HealthChanged(const FOnAttributeChangeData& Data);
	

	UFUNCTION()
	void OnPlayerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	UPlayerInventoryComponent* Inventory;

	

	UFUNCTION()
	void UpdateShieldVal(float NewShield);

	UFUNCTION()
	void UpdateAppliedShieldVal(float NewShield);

	/** Tile on which the player is currently */
	UPROPERTY()
	ATileBase* OnTile;

	/** triggers the player death synced to all clients */
	UFUNCTION(NetMulticast, Unreliable)
	void TriggerPlayerDeath(float RespawnDelay);

	/** triggers the respawn animation */
	UFUNCTION(BlueprintCallable)
	void TriggerPlayerRespawn();

	/** syncs the respawn animation */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_TriggerPlayerRespawn();

	/** blueprint event for player death */
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnPlayerDeath"))
	void ReceivePlayerDeath(float RespawnDelay);

	/** blueprint event for player respawn */
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnPlayerRespawn"))
	void ReceivePlayerRespawn();


	UFUNCTION()
	void ReplaceInventory();
	FTimerHandle InvHandle;
	
};

