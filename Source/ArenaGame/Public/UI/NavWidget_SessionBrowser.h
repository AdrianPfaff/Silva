// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "UINavWidget.h"
#include "OnlineSessionSettings.h"
#include "NavWidget_SessionBrowser.generated.h"

class UNavWidget_ServerRow;
/**
 * Browser widget displaying all found online sessions.
 */
UCLASS()
class ARENAGAME_API UNavWidget_SessionBrowser : public UUINavWidget
{
	GENERATED_BODY()

public:
	void PopulateServerList(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bSuccess);

	UFUNCTION(BlueprintCallable)
	void SearchSessions();
	
	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

	FOnlineSessionSearchResult* GetSessionAtIndex(int32 Index);

protected:

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UScrollBox* ServerList;

	TArray<FOnlineSessionSearchResult> Results;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UNavWidget_ServerRow> ServerRowClass;
};
