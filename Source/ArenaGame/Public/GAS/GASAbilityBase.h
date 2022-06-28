// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ArenaGame/ArenaGame.h"
#include "GASAbilityBase.generated.h"

UENUM(BlueprintType)
enum class ESpellType : uint8
{
	Offensive UMETA(DisplayName="Offensive Spell"),
	Passive UMETA(DisplayName="Passive Spell")
};

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class ARENAGAME_API UGASAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

	public:

	UGASAbilityBase();

	virtual bool CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags) override;
	
	
	virtual bool DependenciesLoaded() const { return true; };
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EGASAbilityInputID AbilityInputID=EGASAbilityInputID::None;


protected:
};
