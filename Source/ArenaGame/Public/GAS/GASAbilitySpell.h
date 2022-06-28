// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "RuneBase.h"
#include "GAS/GASAbilityBase.h"
#include "GASAbilitySpell.generated.h"

/**
 * Base Class for all Player Spells
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class ARENAGAME_API UGASAbilitySpell : public UGASAbilityBase
{
	GENERATED_BODY()


public:

	
	void InjectRunes(TArray<URuneBase*> InRunes);

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

    UFUNCTION(BlueprintCallable)
	ASpell* SpawnSpellActor(TSubclassOf<ASpell> SpellClass, FTransform Transform, AActor* Owner);
	
	UFUNCTION(BlueprintCallable)
	ASpell* SpawnSpellWithoutModifiers(TSubclassOf<ASpell> SpellClass, FTransform Transform, AActor* Owner, FLinearColor Color);

	UFUNCTION(BlueprintCallable)
	void TriggerPassiveEffect();

	UFUNCTION(BlueprintCallable)
	void AppendGameplayEffectTags(const FName& InTag);

	UFUNCTION(BlueprintCallable)
	void RemoveGameplayEffectTag(const FName& InTag);

	//Inventory UI Getters
	UFUNCTION(BlueprintPure)
	 UTexture2D* GetIcon() const;
	UFUNCTION(BlueprintPure)
	 const FText& GetDisplayName() const;
	UFUNCTION(BlueprintPure)
	 const FText& GetDescription() const;
	UFUNCTION(BlueprintPure)
	 int32 GetGold() const;

	UFUNCTION()
	void OnEnemyHitBySpell(AArenaCharacterBase* EnemyHit);

	UFUNCTION()
	void OnEnemyHitBySpellNoModifiers(AArenaCharacterBase* EnemyHit);

	UFUNCTION()
	void OnSpellActorDestroyed(FVector Location);

	UFUNCTION()
	void OnExtraSpellActorDestroyed(FVector Location);
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ESpellType SpellType=ESpellType::Offensive;

	UPROPERTY()
	TArray<URuneBase*> Runes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	UDA_InventoryItemInfo* ItemInfo;

	UPROPERTY(EditAnywhere)
	bool ActivateAbilityOnGranted {false};
	
	/**
	 * Offensive: Base Effect to apply to the hit target
	 * Defensive: Base Effect to apply to self when hit
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> BaseEffectClass;

	void PrepareGameplayEffectAndApply(AArenaCharacterBase* EnemyHit, bool bNoModifiers);
	
	void AddRuneCalculationsToEffect(UGameplayEffect& OutEffect);

	void ApplyPassiveEffectAndRunes();
	
	UPROPERTY()
	TArray<FGameplayTag> TagsToAppend;
	
};
