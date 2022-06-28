// Copyright Runescripters 2021


#include "UI/NavWidget_HUD.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Core/ArenaPlayerCharacter.h"
#include "GAS/GASAttributeSet.h"

static constexpr int32 COOLDOWN_PRECISION=4;

void UNavWidget_HUD::UpdateSpellData()
{
	TArray<UTexture2D*> Icons;
	if (auto PlayerCharacter=Cast<AArenaPlayerCharacter>(GetOwningPlayerPawn()))
	{
		for (auto& SpellSlot: PlayerCharacter->GetInventoryComponent()->GetSpellSlots())
		{
			if (SpellSlot.AbilitySpell)
			{
				Icons.Add(SpellSlot.AbilitySpell->GetIcon());
			} else
			{
				Icons.Add(nullptr);
			}
				
		}
		UpdateIcons(Icons);
	}
}

bool UNavWidget_HUD::Initialize()
{
	bool bSuccess=Super::Initialize();
	
	if (auto PlayerCharacter=Cast<AArenaPlayerCharacter>(GetOwningPlayerPawn()))
	{
		PlayerCharacter->OnHealthChanged.AddDynamic(this, &UNavWidget_HUD::UpdateHealthBar);

		PlayerCharacter->ClampAppliedShieldVal.AddDynamic(this, &UNavWidget_HUD::UpdateShieldBar);
		PlayerCharacter->GetInventoryComponent()->OnGoldChanged.AddLambda([this](int32 Gold){ GoldAmount->SetText(FText::FromString(FString::FromInt(Gold))); });
	}

	return bSuccess;
}

void UNavWidget_HUD::AbilityTriggered(EGASAbilityInputID InputID, float Cooldown)
{
	switch (InputID)
	{
	case EGASAbilityInputID::PrimarySpell:
		PrimaryCooldownMax=Cooldown;
		PrimaryCooldownRemaining=Cooldown;
		PrimaryCooldown->SetText(FText::FromString(FString::SanitizeFloat(Cooldown)));
		PrimarySpell->SetPercent(PrimaryCooldownRemaining/PrimaryCooldownMax);
		break;
	case EGASAbilityInputID::SecondarySpell:
		SecondaryCooldownMax=Cooldown;
		SecondaryCooldownRemaining=Cooldown;
		SecondaryCooldown->SetText(FText::FromString(FString::SanitizeFloat(Cooldown)));
		SecondarySpell->SetPercent(SecondaryCooldownRemaining/SecondaryCooldownMax);
		break;
	case EGASAbilityInputID::TertiarySpell:
		TertiaryCooldownMax=Cooldown;
		TertiaryCooldownRemaining=Cooldown;
		TertiaryCooldown->SetText(FText::FromString(FString::SanitizeFloat(Cooldown)));
		TertiarySpell->SetPercent(TertiaryCooldownRemaining/TertiaryCooldownMax);
		break;
	default:
		UE_LOG(LogSilvaUI, Warning, TEXT("Invalid InputID in Ability UI Trigger"))
		break;
	}
}

void UNavWidget_HUD::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);
	if (!FMath::IsNearlyZero(PrimaryCooldownRemaining))
	{
		PrimaryCooldownRemaining=FMath::Clamp(PrimaryCooldownRemaining-DeltaTime, 0.f, PrimaryCooldownMax);
		FString CooldownString=FString::SanitizeFloat(PrimaryCooldownRemaining);
		CooldownString.LeftInline(COOLDOWN_PRECISION);
		CooldownString+="s";
		PrimaryCooldown->SetText(FText::FromString(CooldownString));
		PrimarySpell->SetPercent(PrimaryCooldownRemaining/PrimaryCooldownMax);
	}
	else
	{
		PrimaryCooldown->SetText(FText());
		PrimarySpell->SetPercent(0.f);
	}
	
	if (!FMath::IsNearlyZero(SecondaryCooldownRemaining))
	{
		SecondaryCooldownRemaining=FMath::Clamp(SecondaryCooldownRemaining-DeltaTime, 0.f, SecondaryCooldownMax);
		FString CooldownString=FString::SanitizeFloat(SecondaryCooldownRemaining);
		CooldownString.LeftInline(COOLDOWN_PRECISION);
		CooldownString+="s";
		SecondaryCooldown->SetText(FText::FromString(CooldownString));
		SecondarySpell->SetPercent(SecondaryCooldownRemaining/SecondaryCooldownMax);
		
	}
	else
	{
		SecondaryCooldown->SetText(FText());
		SecondarySpell->SetPercent(0.f);
	}
	
	if (!FMath::IsNearlyZero(TertiaryCooldownRemaining))
	{
		TertiaryCooldownRemaining=FMath::Clamp(TertiaryCooldownRemaining-DeltaTime, 0.f, TertiaryCooldownMax);
		FString CooldownString=FString::SanitizeFloat(TertiaryCooldownRemaining);
		CooldownString.LeftInline(COOLDOWN_PRECISION);
		CooldownString+="s";
		TertiaryCooldown->SetText(FText::FromString(CooldownString));
		TertiarySpell->SetPercent(TertiaryCooldownRemaining/TertiaryCooldownMax);
	}
	else
	{
		TertiaryCooldown->SetText(FText());
		TertiarySpell->SetPercent(0.f);
	}
}

void UNavWidget_HUD::SetupTimer(FTimespan Duration)
{
	CachedTime=Duration;
	
	PhaseTimer->SetText(FText::AsTimespan(Duration));
	PhaseTimer->SetVisibility(ESlateVisibility::Visible);
	GetWorld()->GetTimerManager().SetTimer(PhaseTimerHandle, this, &UNavWidget_HUD::UpdateTimer, 1.f, true);
}

void UNavWidget_HUD::DisplayMessage(const FString& Msg, float Duration)
{
	PopupText->SetText(FText::FromString(Msg));
	PopupText->SetVisibility(ESlateVisibility::Visible);
	auto Del=FTimerDelegate::CreateLambda([this](){ PopupText->SetVisibility(ESlateVisibility::Collapsed);});
	GetWorld()->GetTimerManager().SetTimer(PopupHandle, Del, Duration, false);
}

void UNavWidget_HUD::UpdateTimer()
{
	CachedTime-=(FTimespan(0, 0, 1));
	if (CachedTime.GetTotalSeconds()<=0)
	{
		PhaseTimer->SetVisibility(ESlateVisibility::Hidden);
		GetWorld()->GetTimerManager().ClearTimer(PhaseTimerHandle);
		
	}
	PhaseTimer->SetText(FText::AsTimespan(CachedTime));
}

void UNavWidget_HUD::UpdateHealthBar(float NewHealth)
{
	HealthBar->SetPercent(NewHealth/100.f);
}

void UNavWidget_HUD::UpdateShieldBar(float NewShield)
{
	ShieldBar->SetPercent(NewShield/100.f);
}
