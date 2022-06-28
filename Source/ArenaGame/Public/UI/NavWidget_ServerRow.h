// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "NavWidget_SessionBrowser.h"
#include "UINavWidget.h"
#include "NavWidget_ServerRow.generated.h"

class UTextBlock;

/**
 * Widget modeling one Server listing inside of the Server Browser.
 */
UCLASS()
class ARENAGAME_API UNavWidget_ServerRow : public UUINavWidget
{
	GENERATED_BODY()

public:
	/** Set up this Server Row using the parameter data */
	void SetServerRow(const FString& UserName, uint8 PlayersConnected, uint16 Ping, uint32 SearchIndex, UNavWidget_SessionBrowser* OwningBrowser);

	UFUNCTION(BlueprintCallable)
	void JoinServer();

	

protected:
	UPROPERTY(BlueprintReadOnly)
	UNavWidget_SessionBrowser* Browser;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UTextBlock* UserNameText;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UTextBlock* PlayerAmountText;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UTextBlock* PingText;

	uint8 MaxPlayers=4;

	uint32 Index;
	
};
