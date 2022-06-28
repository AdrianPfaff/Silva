// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GASAttributeSet.generated.h"

//Using Macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
/**
 * 
 */
UCLASS()
class ARENAGAME_API UGASAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
	public:

	UGASAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	ATTRIBUTE_ACCESSORS(UGASAttributeSet, Health);

	ATTRIBUTE_ACCESSORS(UGASAttributeSet, BaseDamage);

	ATTRIBUTE_ACCESSORS(UGASAttributeSet, DamageReductionMultiplier);

	ATTRIBUTE_ACCESSORS(UGASAttributeSet, MaxHealth);

	ATTRIBUTE_ACCESSORS(UGASAttributeSet, LifestealValue);

	ATTRIBUTE_ACCESSORS(UGASAttributeSet, ExecutionMultiplier);

	ATTRIBUTE_ACCESSORS(UGASAttributeSet, MovementSpeed);
	
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, ShieldValue);

	ATTRIBUTE_ACCESSORS(UGASAttributeSet, AppliedShield);

	ATTRIBUTE_ACCESSORS(UGASAttributeSet, RuneEffectMultiplier);

	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_BaseDamage(const FGameplayAttributeData& OldDamage);

	UFUNCTION()
	virtual void OnRep_DamageReductionMultiplier(const FGameplayAttributeData& OldReductionMultiplier);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	virtual void OnRep_LifestealValue(const FGameplayAttributeData& OldLifestealValue);

	UFUNCTION()
	virtual void OnRep_ExecutionMultiplier(const FGameplayAttributeData& OldExe);

	UFUNCTION()
	virtual void OnRep_MovementSpeed(const FGameplayAttributeData& OldSpeed);
	
	UFUNCTION()
	virtual void OnRep_ShieldValue(const FGameplayAttributeData& OldShield);

	UFUNCTION()
	virtual void OnRep_AppliedShield(const FGameplayAttributeData& OldAppliedShield);
	
	UFUNCTION()
	virtual void OnRep_RuneEffectMultiplier(const FGameplayAttributeData& OldRuneEffect);
	
	
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_BaseDamage)
	FGameplayAttributeData BaseDamage;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_DamageReductionMultiplier)
	FGameplayAttributeData DamageReductionMultiplier;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_LifestealValue)
	FGameplayAttributeData LifestealValue;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_ExecutionMultiplier)
	FGameplayAttributeData ExecutionMultiplier;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MovementSpeed)
	FGameplayAttributeData MovementSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_ShieldValue)
	FGameplayAttributeData ShieldValue;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_AppliedShield)
	FGameplayAttributeData AppliedShield;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_RuneEffectMultiplier)
	FGameplayAttributeData RuneEffectMultiplier;
	
	private:
	
	
	
};
