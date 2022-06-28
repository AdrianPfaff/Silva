// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "UINavWidget.h"
#include "ArenaGame/ArenaGame.h"
#include "NavWidget_HUD.generated.h"

class UTextBlock;
class UProgressBar;

/**
 * 
 */
UCLASS()
class ARENAGAME_API UNavWidget_HUD : public UUserWidget
{
	GENERATED_BODY()

public:

	
	void UpdateSpellData();

	virtual bool Initialize() override;

	UFUNCTION()
	void AbilityTriggered(EGASAbilityInputID InputID, float Cooldown);

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
	
	void SetupTimer(FTimespan Duration);

	void DisplayMessage(const FString& Msg, float Duration=5.f);

protected:

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateIcons(const TArray<UTexture2D*>& Icons);

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UProgressBar* PrimarySpell;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UProgressBar* SecondarySpell;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UProgressBar* TertiarySpell;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UTextBlock* PrimaryCooldown;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UTextBlock* SecondaryCooldown;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UTextBlock* TertiaryCooldown;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UProgressBar* ShieldBar;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UTextBlock* PhaseTimer;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UTextBlock* GoldAmount;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UTextBlock* PopupText;

	UFUNCTION()
	void UpdateTimer();

	FTimerHandle PhaseTimerHandle;

	FTimerHandle PopupHandle;


private:
	float PrimaryCooldownMax=0.f;
	float PrimaryCooldownRemaining=0.f;
	float SecondaryCooldownMax=0.f;
	float SecondaryCooldownRemaining=0.f;
	float TertiaryCooldownMax=0.f;
	float TertiaryCooldownRemaining=0.f;

	float MaxHealth;

	FTimespan CachedTime;

	UFUNCTION()
	void UpdateHealthBar(float NewHealth);

	UFUNCTION()
	void UpdateShieldBar(float NewShield);
};
