// Copyright Runescripters 2021


#include "Core/ArenaGameInstance.h"

#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSessionSettings.h"
#include "ArenaGame/ArenaGame.h"

void UArenaGameInstance::Init()
{
	Super::Init();
	OnlineSubsystem=IOnlineSubsystem::Get();
	checkf(OnlineSubsystem, TEXT("Online Subsystem in Game Instance is invalid!"))
	UE_LOG(LogSilvaOnline, Log, TEXT("Online Subsystem %s found"), *OnlineSubsystem->GetSubsystemName().ToString())
	
	SessionPtr = OnlineSubsystem->GetSessionInterface();
	checkf(SessionPtr, TEXT("Online Subsystem found but SessionPtr is invalid!"))
	SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UArenaGameInstance::OnCreateSessionComplete);
	SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UArenaGameInstance::OnFindSessionsComplete);
	SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UArenaGameInstance::OnJoinSessionCompleted);

	IdentityPtr=OnlineSubsystem->GetIdentityInterface();
	checkf(IdentityPtr, TEXT("Online Subsystem found but IdentityPtr is invalid!"))
	IdentityPtr->OnLoginCompleteDelegates->AddUObject(this, &UArenaGameInstance::OnLoginComplete);

	
	
}

void UArenaGameInstance::Login()
{
		FOnlineAccountCredentials Credentials;
		Credentials.Id = FString();
		Credentials.Token = FString();
		Credentials.Type = FString("accountportal");
		IdentityPtr->Login(0, Credentials);
}

void UArenaGameInstance::BeginDestroy()
{
	DestroySession();
	Super::BeginDestroy();
}

void UArenaGameInstance::JoinGameSession(const FOnlineSessionSearchResult& SessionResult)
{
	UE_LOG(LogSilvaOnline, Log, TEXT("Begin joining session..."))
	SessionPtr->JoinSession(0, NAME_GameSession, SessionResult);
}

void UArenaGameInstance::BeginCreateSession(bool bIsPublic)
{
	FOnlineSessionSettings Settings;
	Settings.NumPrivateConnections = 0;
	Settings.NumPublicConnections = 4;
	Settings.bAllowInvites = true;
	Settings.bAllowJoinInProgress = false;
	Settings.bAllowJoinViaPresence = true;
	Settings.bAllowJoinViaPresenceFriendsOnly = true;
	Settings.bIsDedicated = false;
	Settings.bUsesPresence = true;
	Settings.bIsLANMatch = false;
	Settings.bShouldAdvertise = true;
	Settings.Set(SETTING_MAPNAME, FString("Lobby"), EOnlineDataAdvertisementType::ViaOnlineService);
	UE_LOG(LogSilvaOnline, Log, TEXT("Creating session"));
	SessionPtr->CreateSession(0, NAME_GameSession, Settings);
	
}

void UArenaGameInstance::CancelSessionCreation()
{
	FString SessionName=IdentityPtr->GetPlayerNickname(0);
	SessionPtr->DestroySession(NAME_GameSession);
}

void UArenaGameInstance::FindSessions()
{
	OnlineSearch=MakeShareable(new FOnlineSessionSearch());
	//Query Settings seem to be bugged?
	//OnlineSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	OnlineSearch->TimeoutInSeconds=15;
	OnlineSearch->bIsLanQuery=false;
	OnlineSearch->MaxSearchResults=100;
	UE_LOG(LogSilvaOnline, Log, TEXT("Beginning Search for Sessions"))
	SessionPtr->FindSessions(0,OnlineSearch.ToSharedRef());
}

void UArenaGameInstance::OnFindSessionsComplete(bool bSuccess)
{
	UE_LOG(LogSilvaOnline, Log, TEXT("Session Search Complete"))
	GameInstanceOnSessionSearchCompleteDelegates.Broadcast(OnlineSearch->SearchResults, bSuccess);
}

void UArenaGameInstance::OnCreateSessionComplete(FName SessionName, bool bSuccess)
{
	if (!bSuccess)
	{
		UE_LOG(LogSilvaOnline, Error, TEXT("Create Online Session failed"))
	}
	else
	{
		UE_LOG(LogSilvaOnline, Log, TEXT("Successfully created Online Session"))
	}
	GetWorld()->ServerTravel("/Game/Arena/Maps/Dungeon?listen");
	GameInstanceOnCreateSessionComplete.Broadcast(SessionName, bSuccess);
}

void UArenaGameInstance::OnLoginComplete(int LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	GameInstanceOnLoginCompleteDelegates.Broadcast(LocalUserNum, bWasSuccessful, Error);
	if (!bWasSuccessful)
	{
		UE_LOG(LogSilvaOnline, Error, TEXT("Logging in failed: %s"), *Error)
	}
	else
	{
		UE_LOG(LogSilvaOnline, Log, TEXT("Logging in successful"))
	}
}

void UArenaGameInstance::DestroySession()
{
	if (SessionPtr.IsValid())
	{
		SessionPtr->DestroySession(NAME_GameSession);
	}
	
}

void UArenaGameInstance::OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
FString Status=TEXT("Cheese");
	switch (Result) { case EOnJoinSessionCompleteResult::Success:
		Status=TEXT("Success");
		break;
	case EOnJoinSessionCompleteResult::SessionIsFull:
		Status=TEXT("Session Full");
		break;
	case EOnJoinSessionCompleteResult::SessionDoesNotExist:
		Status=TEXT("Session does not exist");
		break;
	case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
		Status=TEXT("Could not retrieve address");
		break;
	case EOnJoinSessionCompleteResult::AlreadyInSession:
		Status=TEXT("Already in session");
		break;
	case EOnJoinSessionCompleteResult::UnknownError:
		Status=TEXT("Unknown Error");
		break;
	}
	UE_LOG(LogSilvaOnline, Log, TEXT("Session Join Completed: %s"), *Status)
// ReSharper disable once CppExpressionWithoutSideEffects
	GIOnJoinSessionComplete.Broadcast(SessionName, Result);

	FString ConnectString;
	SessionPtr->GetResolvedConnectString(NAME_GameSession, ConnectString);
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	UE_LOG(LogSilvaOnline, Display, TEXT("Trying to travel to %s"), *ConnectString)
	PlayerController->ClientTravel(ConnectString, TRAVEL_Absolute);
}
