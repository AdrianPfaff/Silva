// Copyright Runescripters 2021


#include "GAS/GASAttributeSet.h"

#include "Net/UnrealNetwork.h"

UGASAttributeSet::UGASAttributeSet()
{
	Health.SetBaseValue(69.f);
	Health.SetCurrentValue(69.f);
	MaxHealth.SetBaseValue(69.f);
	MaxHealth.SetCurrentValue(69.f);
}

void UGASAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGASAttributeSet, Health, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UGASAttributeSet, BaseDamage, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UGASAttributeSet, DamageReductionMultiplier, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UGASAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UGASAttributeSet, LifestealValue, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UGASAttributeSet, ExecutionMultiplier, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UGASAttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UGASAttributeSet, ShieldValue, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UGASAttributeSet, AppliedShield, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UGASAttributeSet, RuneEffectMultiplier, COND_None, REPNOTIFY_Always);
}

void UGASAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASAttributeSet, Health, OldHealth);
}

void UGASAttributeSet::OnRep_BaseDamage(const FGameplayAttributeData& OldDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASAttributeSet, BaseDamage, OldDamage);
}

void UGASAttributeSet::OnRep_DamageReductionMultiplier(const FGameplayAttributeData& OldReductionMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASAttributeSet, DamageReductionMultiplier, OldReductionMultiplier);
}

void UGASAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASAttributeSet, MaxHealth, OldMaxHealth);
}

void UGASAttributeSet::OnRep_LifestealValue(const FGameplayAttributeData& OldLifestealValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASAttributeSet, LifestealValue, OldLifestealValue);
}

void UGASAttributeSet::OnRep_ExecutionMultiplier(const FGameplayAttributeData& OldExe)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASAttributeSet, ExecutionMultiplier, OldExe);
}

void UGASAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASAttributeSet, MovementSpeed, OldSpeed);
}

void UGASAttributeSet::OnRep_ShieldValue(const FGameplayAttributeData& OldShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASAttributeSet, ShieldValue, OldShield);
}

void UGASAttributeSet::OnRep_AppliedShield(const FGameplayAttributeData& OldAppliedShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASAttributeSet, AppliedShield, OldAppliedShield);
}

void UGASAttributeSet::OnRep_RuneEffectMultiplier(const FGameplayAttributeData& OldRuneEffect)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASAttributeSet, RuneEffectMultiplier, OldRuneEffect);
}
