// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "ArenaGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FGameInstanceOnLoginComplete, int, LocalUserNum, bool, bWasSuccessful, const FString&, Error);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGameInstanceOnCreateSessionComplete, FName, SessionName, bool, bSuccess);
DECLARE_MULTICAST_DELEGATE_TwoParams(FGameInstanceOnSessionSearchComplete,  const TArray<FOnlineSessionSearchResult>& SessionResults, bool bSuccess);
DECLARE_MULTICAST_DELEGATE_TwoParams(FGameInstanceOnJoinSessionComplete,  FName SessionName, EOnJoinSessionCompleteResult::Type Result);

/**
 * @brief The game instance houses all network related functionality that needs to be persistent across the whole game,
 * independent of the current match
 */
UCLASS()
class ARENAGAME_API UArenaGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	/** Initializes the instance, setting up bindings for online delegates */
	virtual void Init() override;

	/** Function called to start the login process using EOS */
	UFUNCTION(BlueprintCallable)
	void Login();

	/** Before tearing down the instance cleans up running sessions */
	virtual void BeginDestroy() override;


	
	/**
	 * @brief Starts the process of joining a game found from the server browser
	 * @param SessionResult the session to join
	 */
	void JoinGameSession(const FOnlineSessionSearchResult& SessionResult);

	/** Delegate called when done joining a session */
	FGameInstanceOnJoinSessionComplete GIOnJoinSessionComplete;

	/**
	 * @brief Begins creating a new session, calling back on OnCreateSessionComplete
	 * @param bIsPublic Whether the Match is visible in the Server Browser
	 */
	UFUNCTION(BlueprintCallable)
	void BeginCreateSession( bool bIsPublic);

	/** aborts creating a session */
	UFUNCTION(BlueprintCallable)
	void CancelSessionCreation();

	/** begins searching for sessions, calling back on @property GameInstanceOnSessionSearchCompleteDelegates */
	void FindSessions();

	/** helper to trigger Session Search Complete */
	UFUNCTION()
	void OnFindSessionsComplete(bool bSuccess);

	/** executed when done creating a new session */
	UFUNCTION()
	void OnCreateSessionComplete(FName SessionName, bool bSuccess);

	/** callback for logging in after authorizing with EOS */
	void OnLoginComplete(int LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	/** Property called when done searching for sessions */
	FGameInstanceOnSessionSearchComplete GameInstanceOnSessionSearchCompleteDelegates;

	/** begins tearing down a running session */
	UFUNCTION(BlueprintCallable)
	void DestroySession();
	
	//BP Delegates
	UPROPERTY(BlueprintAssignable, DisplayName=OnLoginComplete)
	FGameInstanceOnLoginComplete GameInstanceOnLoginCompleteDelegates;
	UPROPERTY(BlueprintAssignable, DisplayName=OnCreateSessionComplete)
	FGameInstanceOnCreateSessionComplete GameInstanceOnCreateSessionComplete;
	
protected:
	/** Cached pointer to the Online Subsystem */
	class IOnlineSubsystem* OnlineSubsystem;
	/** Cached pointer to the Online Session System */
	IOnlineSessionPtr SessionPtr;
	/** Cached pointer to the Login System */
	IOnlineIdentityPtr IdentityPtr;

	/** results to the online search is stored here */
	TSharedPtr<FOnlineSessionSearch> OnlineSearch;

	/** called when done joining a session */
	void OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	
	
};