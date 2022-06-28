// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "GAS/GASAttributeSet.h"
#include "ShieldValueMMC.generated.h"

/**
 * 
 */
UCLASS()
class ARENAGAME_API UShieldValueMMC : public UGameplayModMagnitudeCalculation
{
	GENERATED_UCLASS_BODY()

	public:
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;


	FGameplayEffectAttributeCaptureDefinition ShieldValueDef;

	FGameplayEffectAttributeCaptureDefinition BaseDamageDef;

	FGameplayEffectAttributeCaptureDefinition MaxHealthDef;
};
