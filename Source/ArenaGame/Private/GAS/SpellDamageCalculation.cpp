// Copyright Runescripters 2021


#include "GAS/SpellDamageCalculation.h"

#include "ArenaGame/ArenaGame.h"
#include "Core/ArenaPlayerCharacter.h"

USpellDamageCalculation::USpellDamageCalculation(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	TargetAttribute TargetAttribute;

	RelevantAttributesToCapture.Add(TargetAttribute.HealthDef);//RelevantAttributesToCapture is the array that contains all attributes you wish to capture, without exceptions.
#if	WITH_EDITORONLY_DATA
	InvalidScopedModifierAttributes.Add(TargetAttribute.HealthDef);
#endif
	

	RelevantAttributesToCapture.Add(TargetAttribute.MaxHealthDef);
	RelevantAttributesToCapture.Add(TargetAttribute.BaseDamageDef);
	RelevantAttributesToCapture.Add(TargetAttribute.DamageReductionMultiplierDef);
	RelevantAttributesToCapture.Add(TargetAttribute.LifestealValueDef);
	RelevantAttributesToCapture.Add(TargetAttribute.ExecutionMultiplierDef);
	RelevantAttributesToCapture.Add(TargetAttribute.AppliedShieldDef);
	
	SourceAttributeCapture SourceAttributeCapture;
	RelevantAttributesToCapture.Add(SourceAttributeCapture.LifestealValueDef);
	RelevantAttributesToCapture.Add(SourceAttributeCapture.BaseDamageDef);
	RelevantAttributesToCapture.Add(SourceAttributeCapture.HealthDef);
	RelevantAttributesToCapture.Add(SourceAttributeCapture.MaxHealthDef);
	RelevantAttributesToCapture.Add(SourceAttributeCapture.DamageReductionMultiplierDef);
}

void USpellDamageCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	//Set all Variables and References for SpellCapture
	TargetAttribute TargetAttribute;
	//Set all Variables and References for SecondCapture
	SourceAttributeCapture SourceAttributeCapture;

	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();

	UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->GetAvatarActor() : nullptr;

	AActor* TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluationParameters;

	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
	

	//Capture Attributes: TARGET VALUES
	float TargetHealth = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(TargetAttribute.HealthDef, EvaluationParameters, TargetHealth);

	float TargetMaxHealth = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(TargetAttribute.MaxHealthDef, EvaluationParameters, TargetMaxHealth);

	float TargetExecutionMultiplier = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(TargetAttribute.ExecutionMultiplierDef, EvaluationParameters, TargetExecutionMultiplier);
	
	float TargetReductionMultiplier = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(TargetAttribute.DamageReductionMultiplierDef, EvaluationParameters, TargetReductionMultiplier);

	float TargetLifesteal = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(TargetAttribute.LifestealValueDef, EvaluationParameters, TargetLifesteal);

	float TargetShield = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(TargetAttribute.AppliedShieldDef, EvaluationParameters, TargetShield);
	

	//Capture Attributes: SOURCE VALUES
	float SourceBaseDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(SourceAttributeCapture.BaseDamageDef, EvaluationParameters, SourceBaseDamage);
	
	
	float SourceLifeSteal = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(SourceAttributeCapture.LifestealValueDef, EvaluationParameters, SourceLifeSteal);

	float DamageDone = SourceBaseDamage * TargetReductionMultiplier;
	
	//Get all ability Tags
	FGameplayTagContainer TagCont;
	Spec.GetAllAssetTags(TagCont);
	

	for (auto& Modifier: Spec.Def->Modifiers)
	{
		float Magnitude = 1.f;
		if (Modifier.Attribute.GetName() == "BaseDamage")
		{
			Modifier.ModifierMagnitude.GetStaticMagnitudeIfPossible(0, Magnitude);
		}
		DamageDone *= Magnitude;
	}

	//Apply Powerflux Stacks
	if (TagCont.HasTag(FGameplayTag::RequestGameplayTag(FName("Powerflux"))))
	{
		AArenaPlayerCharacter* TargetPlayer = Cast<AArenaPlayerCharacter>(SourceActor);
		if (TargetPlayer)
		{
			DamageDone *= (TargetPlayer->PowerfluxStacks  * 0.1 + 1.0f);
		}
	}
	
	//Apply Execute Multiplicator
	if (TagCont.HasTag(FGameplayTag::RequestGameplayTag(FName("Execute"))))
	{
		UE_LOG(LogSilvaGAS, Warning, TEXT("Exe Val: %f"), TargetExecutionMultiplier);
		if (TargetHealth <= (TargetMaxHealth * 0.5))
		{
			DamageDone *= TargetExecutionMultiplier;
		}
	}
	
	DamageDone = FMath::Min<float>(DamageDone, TargetHealth);

	//Prevent Death by Tag
	if (TargetTags->HasTag(FGameplayTag::RequestGameplayTag(FName("CannotDie"))))
	{
		if (DamageDone >= TargetHealth)
		{
			DamageDone = TargetHealth - 1.0f;
		}
	}

	//Prevent Damage with Tag "Deny Damage"
	if (TargetTags->HasTag(FGameplayTag::RequestGameplayTag(FName("DenyDamage"))) && DamageDone > 0.f)
	{		
		//TargetAbilitySystemComponent->ComponentTags.Remove("DenyDamage");
		if (ExecutionParams.GetTargetAbilitySystemComponent())
		{
			TargetAbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("DenyDamage")));
		}
		
	
		DamageDone = 0.f;
	}
	
	//Apply Damage to Shield or Health
	if (DamageDone > 0.f)
	{
		if (TargetShield > 0.f)
		{

			if (TargetShield >= DamageDone)
			{
				
				OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(TargetAttribute.AppliedShieldProperty, EGameplayModOp::Additive, -DamageDone));
			}
			else
			{
				DamageDone = DamageDone - TargetShield;				
				
				OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(TargetAttribute.AppliedShieldProperty, EGameplayModOp::Additive, -TargetShield));
				OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(TargetAttribute.HealthProperty, EGameplayModOp::Additive, -DamageDone));
			}
			
		}
		else
		{
			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(TargetAttribute.HealthProperty, EGameplayModOp::Additive, -DamageDone));
		}
		
	}

	

	
	
	
	//Apply Passive Lifesteal
	if (SourceTags->HasTag(FGameplayTag::RequestGameplayTag(FName("ApplyPassiveLifesteal"))))
	{
		UGameplayEffect* GELifesteal = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("Lifesteal")));
		GELifesteal->DurationPolicy = EGameplayEffectDurationType::Instant;

		GELifesteal->StackingType =  EGameplayEffectStackingType::AggregateByTarget;
		int32 Idx = GELifesteal->Modifiers.Num();
		GELifesteal->Modifiers.SetNum(Idx + 1);
		
		float Lifesteal = DamageDone * SourceLifeSteal;
		
		FGameplayModifierInfo& Info = GELifesteal->Modifiers[Idx];
		Info.ModifierMagnitude = FScalableFloat(Lifesteal);
		Info.ModifierOp = EGameplayModOp::Additive;
		Info.Attribute = UGASAttributeSet::GetHealthAttribute();

		if (ExecutionParams.GetSourceAbilitySystemComponent())
		{
			SourceAbilitySystemComponent->ApplyGameplayEffectToSelf(GELifesteal, 1.0f, SourceAbilitySystemComponent->MakeEffectContext());
		}
		
	}

	//Apply Active Lifesteal
    	
    		
    		UGameplayEffect* GELifesteal = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("Lifesteal")));
    		GELifesteal->DurationPolicy = EGameplayEffectDurationType::Instant;
    
    		GELifesteal->StackingType =  EGameplayEffectStackingType::AggregateByTarget;
    		int32 Idx = GELifesteal->Modifiers.Num();
    		GELifesteal->Modifiers.SetNum(Idx + 1);
    
    		float LifestealMod = 0.f;
    		for (auto& Modifier: Spec.Def->Modifiers)
    		{
    			float Magnitude = 0.f;
    			if (Modifier.Attribute.GetName() == "LifestealValue")
    			{
    				Modifier.ModifierMagnitude.GetStaticMagnitudeIfPossible(0, Magnitude);
    			}
    			LifestealMod += Magnitude;
    		}
	
    		float Lifesteal = DamageDone * LifestealMod;
    		
    		FGameplayModifierInfo& Info = GELifesteal->Modifiers[Idx];
    		Info.ModifierMagnitude = FScalableFloat(Lifesteal);
    		Info.ModifierOp = EGameplayModOp::Additive;
    		Info.Attribute = UGASAttributeSet::GetHealthAttribute();

	if (ExecutionParams.GetSourceAbilitySystemComponent())
	{
		SourceAbilitySystemComponent->ApplyGameplayEffectToSelf(GELifesteal, 1.0f, SourceAbilitySystemComponent->MakeEffectContext());
	}
	
}
