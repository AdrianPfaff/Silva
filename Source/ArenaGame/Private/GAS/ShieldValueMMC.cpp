// Copyright Runescripters 2021


#include "GAS/ShieldValueMMC.h"

#include "ArenaGame/ArenaGame.h"


UShieldValueMMC::UShieldValueMMC(const FObjectInitializer& ObjectInit) : Super(ObjectInit)
{
	ShieldValueDef.AttributeToCapture = UGASAttributeSet::GetShieldValueAttribute();
	ShieldValueDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	ShieldValueDef.bSnapshot = true;

	BaseDamageDef.AttributeToCapture = UGASAttributeSet::GetBaseDamageAttribute();
	BaseDamageDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	BaseDamageDef.bSnapshot = true;

	MaxHealthDef.AttributeToCapture = UGASAttributeSet::GetMaxHealthAttribute();
	MaxHealthDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	MaxHealthDef.bSnapshot = true;

	RelevantAttributesToCapture.Add(ShieldValueDef);
	RelevantAttributesToCapture.Add(BaseDamageDef);
	RelevantAttributesToCapture.Add(MaxHealthDef);
}

float UShieldValueMMC::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float ShieldVal = 0.f;
	GetCapturedAttributeMagnitude(ShieldValueDef, Spec, EvaluationParameters, ShieldVal);
	
	float BaseDamage = 0.f;
	GetCapturedAttributeMagnitude(BaseDamageDef, Spec, EvaluationParameters, BaseDamage);

	float MaxHealth = 0.f;
	GetCapturedAttributeMagnitude(MaxHealthDef, Spec, EvaluationParameters, MaxHealth);

	float ShieldingDone = 0.f;
	if (SourceTags->HasTag(FGameplayTag::RequestGameplayTag(FName("UseMaxHealthShield"))))
	{
		ShieldingDone = MaxHealth * 0.1;
	}
	else
	{
		ShieldingDone = BaseDamage;
	}
	

	for (auto& Modifier: Spec.Def->Modifiers)
	{
		float Magnitude = 1.f;
		Modifier.ModifierMagnitude.GetStaticMagnitudeIfPossible(0, Magnitude);
		ShieldingDone *= Magnitude;
	}
	
	UE_LOG(LogSilvaGAS, Warning, TEXT("ShieldVal: %f"), ShieldVal);
	UE_LOG(LogSilvaGAS, Warning, TEXT("ShieldingDone: %f"), ShieldingDone);
	
	
	
	return ShieldingDone;
}
