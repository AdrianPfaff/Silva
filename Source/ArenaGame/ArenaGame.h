// Copyright Team Arena 2021

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSilvaGame, Log, All)
DECLARE_LOG_CATEGORY_EXTERN(LogSilvaOnline, Log, All)
DECLARE_LOG_CATEGORY_EXTERN(LogSilvaUI, Log, All)
DECLARE_LOG_CATEGORY_EXTERN(LogSilvaGAS, Log, All)

UENUM(BlueprintType)
enum class EGASAbilityInputID : uint8
{
	None,
	Confirm,
	Cancel,
	PrimarySpell,
	SecondarySpell,
	TertiarySpell
};