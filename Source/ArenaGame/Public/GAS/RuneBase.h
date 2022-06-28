// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "GASAbilityBase.h"
#include "Enemies/ArenaEnemyCharacterBase.h"
#include "RuneBase.generated.h"

class UDA_InventoryItemInfo;
class UGASAbilitySpell;
class ASpell;
/**
 * 
 */
UENUM()
enum class ERecursiveType { FullRecursive, OffensiveRecursive, PassiveRecursive, NonRecursive };

UCLASS(Blueprintable, BlueprintType, Abstract)
class ARENAGAME_API URuneBase : public UObject
{
	GENERATED_BODY()

public:

	virtual bool IsSupportedForNetworking() const override;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	UFUNCTION(BlueprintImplementableEvent)
	void PreActorSpawn(ASpell* ActorToSpawn, ESpellType SpellType, UGASAbilitySpell* InSpell, uint8 RuneIndex);

	UFUNCTION(BlueprintImplementableEvent)
	void PostActorSpawn(ASpell* SpawnedActor, ESpellType SpellType, UGASAbilitySpell* InSpell, uint8 RuneIndex);

	UFUNCTION(BlueprintImplementableEvent)
	void OnEnemyHit(AArenaCharacterBase* HitEnemy, ESpellType SpellType, UGASAbilitySpell* InSpell, uint8 RuneIndex);

	UFUNCTION(BlueprintImplementableEvent)
	void OnDestroy(ESpellType SpellType, UGASAbilitySpell* InSpell, uint8 RuneIndex, FVector Location);

	UFUNCTION(BlueprintImplementableEvent)
	void OnPassiveEffectTrigger(UGASAbilitySpell* InSpell);
	
	const TArray<FGameplayModifierInfo>& GetOffensiveModifiers() const { return OffensiveModifiers; }
	const TArray<FGameplayModifierInfo>& GetPassiveModifiers() const { return PassiveModifiers; }

	UFUNCTION(BlueprintPure)
	AActor* GetOwner() const { return Owner; }

	void SetOwner(AActor* InOwner) { Owner=InOwner; }

	//Inventory UI Getters
	UFUNCTION(BlueprintPure)
	 UTexture2D* GetIcon() const;
	UFUNCTION(BlueprintPure)
	 const FText& GetDisplayName() const;
	UFUNCTION(BlueprintPure)
	 const FText& GetDescription() const;
	UFUNCTION(BlueprintPure)
	 int32 GetGold() const;

	UPROPERTY(EditAnywhere)
	ERecursiveType RecursiveType {ERecursiveType::NonRecursive};

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	UDA_InventoryItemInfo* ItemInfo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FGameplayModifierInfo> OffensiveModifiers;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FGameplayModifierInfo> PassiveModifiers;

	UPROPERTY(Replicated)
	AActor* Owner;

	
};
