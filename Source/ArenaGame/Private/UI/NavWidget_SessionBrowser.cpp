// Copyright Runescripters 2021


#include "UI/NavWidget_SessionBrowser.h"

#include "ArenaGame/ArenaGame.h"
#include "Components/ScrollBox.h"
#include "Core/ArenaGameInstance.h"
#include "UI/NavWidget_ServerRow.h"

void UNavWidget_SessionBrowser::PopulateServerList(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bSuccess)
{
	FString SessionName=TEXT("Public Session");
	Results=SessionResults;
	ServerList->ClearChildren();
	UE_LOG(LogSilvaOnline, Log, TEXT("Found %i Servers"), Results.Num())
	uint32 Index=0;
	for (auto& Result : Results)
	{
		uint8 PlayersConnected=Result.Session.SessionSettings.NumPublicConnections-Result.Session.NumOpenPublicConnections;
		UNavWidget_ServerRow* Widget= CreateWidget<UNavWidget_ServerRow>(this, ServerRowClass);
		ServerList->AddChild(Widget);
		Widget->SetServerRow(SessionName, PlayersConnected, Result.PingInMs, Index, this);
		++Index;
	}
	UE_LOG(LogSilvaOnline, Log, TEXT("Populating ServerList"))
}

void UNavWidget_SessionBrowser::SearchSessions()
{
	auto GameInstance=Cast<UArenaGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->GameInstanceOnSessionSearchCompleteDelegates.AddUObject(this, &UNavWidget_SessionBrowser::PopulateServerList);
		GameInstance->FindSessions();
	}
}

bool UNavWidget_SessionBrowser::Initialize()
{
	Super::Initialize();
	return true;
}

void UNavWidget_SessionBrowser::NativeConstruct()
{
	Super::NativeConstruct();
	SearchSessions();
}

FOnlineSessionSearchResult* UNavWidget_SessionBrowser::GetSessionAtIndex(int32 Index)
{
	if (!Results.IsValidIndex(Index)) return nullptr;
	
	return &Results[Index]; 
}
