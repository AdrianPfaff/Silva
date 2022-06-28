// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_InventoryItemInfo.generated.h"

/**
 * Data Asset containing display information for runes and spells. Used to conveniently set up data to display inside the
 * inventory instead of hard-binding to the UObject.
 */
UCLASS(BlueprintType)
class ARENAGAME_API UDA_InventoryItemInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Icon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(MultiLine=true))
	FText Description;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int32 ItemPrice = 0;
};
