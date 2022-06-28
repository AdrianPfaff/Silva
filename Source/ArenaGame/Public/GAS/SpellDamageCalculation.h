// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GASAttributeSet.h"
#include "SpellDamageCalculation.generated.h"

/**
 * 
 */
UCLASS()
class ARENAGAME_API USpellDamageCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_UCLASS_BODY()

	public:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	
};

struct TargetAttribute
{
	//The DECLARE_ATTRIBUTE_CAPTUREDEF macro actually only declares two variables. The variable names are dependent on the input, however. Here they will be HealthProperty(which is a UPROPERTY pointer)
	//and HealthDef(which is a FGameplayEffectAttributeCaptureDefinition).
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxHealth);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageReductionMultiplier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LifestealValue)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ExecutionMultiplier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(AppliedShield);

	TargetAttribute()
	{
		// We define the values of the variables we declared now. In this example, HealthProperty will point to the Health attribute in the UGASAttributeSet on the receiving target of this execution. The last parameter is a bool, and determines if we snapshot the attribute's value at the time of definition.
		DEFINE_ATTRIBUTE_CAPTUREDEF(UGASAttributeSet, Health, Target, false)

		DEFINE_ATTRIBUTE_CAPTUREDEF(UGASAttributeSet, MaxHealth, Target, false)

		DEFINE_ATTRIBUTE_CAPTUREDEF(UGASAttributeSet, BaseDamage, Target, false)

		DEFINE_ATTRIBUTE_CAPTUREDEF(UGASAttributeSet, DamageReductionMultiplier, Target, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UGASAttributeSet, LifestealValue, Target, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UGASAttributeSet, ExecutionMultiplier, Target,false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UGASAttributeSet, AppliedShield, Target,false);
	}
	
};

struct SourceAttributeCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);

	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxHealth);

	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageReductionMultiplier);
	
	DECLARE_ATTRIBUTE_CAPTUREDEF(LifestealValue);

	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseDamage);
	

	SourceAttributeCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UGASAttributeSet, LifestealValue, Source, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UGASAttributeSet, BaseDamage, Source, true);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UGASAttributeSet, Health, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UGASAttributeSet, MaxHealth, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UGASAttributeSet, DamageReductionMultiplier, Source, false);

		
	}
};