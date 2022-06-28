// Copyright Runescripters 2021


#include "UI/NavWidget_ServerRow.h"

#include "Components/TextBlock.h"
#include "Core/ArenaGameInstance.h"
#include "ArenaGame/ArenaGame.h"
#include "UI/NavWidget_SessionBrowser.h"

void UNavWidget_ServerRow::SetServerRow(const FString& UserName, uint8 PlayersConnected, uint16 Ping, uint32 SearchIndex, UNavWidget_SessionBrowser* OwningBrowser)
{
	Browser=OwningBrowser;
	Index=SearchIndex;
	UserNameText->SetText(FText::FromString(UserName));
	UE_LOG(LogSilvaOnline, Display, TEXT("Server row for player %s found"), *UserName)
	FText Players=FText::FromString(FString::FromInt(PlayersConnected) + "/" + FString::FromInt(MaxPlayers));
	PlayerAmountText->SetText(Players);
	PingText->SetText(FText::FromString(FString::FromInt(Ping)+ "ms"));
}

void UNavWidget_ServerRow::JoinServer()
{
	if(Browser)
	{
		auto SessionInfo=Browser->GetSessionAtIndex(Index);
		if (SessionInfo)
		{
			auto GI=Cast<UArenaGameInstance>(GetGameInstance());
			if (GI)
			{
			    UE_LOG(LogSilvaUI, Display, TEXT("Joining Game Session"))
				GI->JoinGameSession(*SessionInfo);
			}
		}
	}
}
