// Copyright Runescripters 2021


#include "GAS/PreparationSpellMMC.h"
#include "ArenaGame/ArenaGame.h"

UPreparationSpellMMC::UPreparationSpellMMC(const FObjectInitializer& ObjectInit) : Super (ObjectInit)
{
	DamageReductionMultiplierDef.AttributeToCapture = UGASAttributeSet::GetDamageReductionMultiplierAttribute();
	DamageReductionMultiplierDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	DamageReductionMultiplierDef.bSnapshot = true;

	RelevantAttributesToCapture.Add(DamageReductionMultiplierDef);
}

float UPreparationSpellMMC::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	float ReductionCalc = 0.f;

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float TargetReduction = 0.f;
	GetCapturedAttributeMagnitude(DamageReductionMultiplierDef, Spec, EvaluationParameters, TargetReduction);
	
	float MagnitudeMultiplier = 1.f;
	
	for (auto& Modifier: Spec.Def->Modifiers)
	{
		float Magnitude = 1.f;
		Modifier.ModifierMagnitude.GetStaticMagnitudeIfPossible(0, Magnitude);
		MagnitudeMultiplier *= Magnitude;
	}

	ReductionCalc = (1.f+(0.5 * MagnitudeMultiplier)) * TargetReduction;
	
	return ReductionCalc;
}
