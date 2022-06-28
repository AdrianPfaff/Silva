// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GASAttributeSet.h"
#include "HealValueExecutionCalculation.generated.h"

/**
 * 
 */
UCLASS()
class ARENAGAME_API UHealValueExecutionCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_UCLASS_BODY()

	public:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};

struct HealSpellCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);

	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxHealth);

	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseDamage);

	HealSpellCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UGASAttributeSet, Health, Source, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UGASAttributeSet, MaxHealth, Source, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UGASAttributeSet, BaseDamage, Source, false);
	}
};
