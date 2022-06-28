// Copyright Runescripters 2021


#include "GAS/HealValueExecutionCalculation.h"

#include "ArenaGame/ArenaGame.h"

UHealValueExecutionCalculation::UHealValueExecutionCalculation(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	HealSpellCapture Capture;

	RelevantAttributesToCapture.Add(Capture.HealthDef);
	RelevantAttributesToCapture.Add(Capture.BaseDamageDef);
	RelevantAttributesToCapture.Add(Capture.MaxHealthDef);
}

void UHealValueExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	HealSpellCapture Capture;

	UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;

	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = SourceTags;

	float Health = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(Capture.HealthDef, EvaluationParameters, Health);

	float MaxHealth = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(Capture.MaxHealthDef, EvaluationParameters, MaxHealth);

	float BaseDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(Capture.BaseDamageDef, EvaluationParameters, BaseDamage);

	float HealingDone = BaseDamage;
	
	for (auto& Modifier: Spec.Def->Modifiers)
	{
		float Magnitude = 0.f;
		Modifier.ModifierMagnitude.GetStaticMagnitudeIfPossible(0, Magnitude);
		HealingDone *= Magnitude;
	}
	
	HealingDone = FMath::Clamp(HealingDone, 0.f, (MaxHealth-Health));

	
	if (HealingDone > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(Capture.HealthProperty, EGameplayModOp::Additive, HealingDone));
	}
}
