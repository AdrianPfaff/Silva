// Copyright Runescripters 2021


#include "Core/DuelGameMode.h"

#include "Core/ArenaGameState.h"
#include "Core/ArenaPlayerCharacter.h"
#include "Core/ArenaPlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "Gameplay/ArenaDoor.h"
#include "Gameplay/DuelPlayerStart.h"

ADuelGameMode::ADuelGameMode()
{
	DuelPlayerStarts.Add(nullptr);
	DuelPlayerStarts.Add(nullptr);
	DuelPlayerStarts.Add(nullptr);
	DuelPlayerStarts.Add(nullptr);

	Doors.Add(nullptr);
	Doors.Add(nullptr);
	Doors.Add(nullptr);
	Doors.Add(nullptr);
}

void ADuelGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(DuelHandle, this, &ADuelGameMode::InitializeDuels, MaxWaitTime);
}

void ADuelGameMode::GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList)
{
	Super::GetSeamlessTravelActorList(bToTransition, ActorList);
	ActorList.Add(GetGameState<AArenaGameState>());
	
}

void ADuelGameMode::RegisterSpawn(ADuelPlayerStart* PlayerStart, uint8 StartIndex)
{
	DuelPlayerStarts[StartIndex]=PlayerStart;
}

void ADuelGameMode::RegisterDoor(AArenaDoor* Door, uint8 DoorIndex)
{
	Doors[DoorIndex]=Door;
}

void ADuelGameMode::GenericPlayerInitialization(AController* C)
{
	Super::GenericPlayerInitialization(C);

	if (auto ArenaPlayer=Cast<AArenaPlayerController>(C))
	{
		Players.Add(FDuelPlayerState(ArenaPlayer));
	}
}

void ADuelGameMode::HandlePlayerDeath(ACharacter* DeadPlayer, float& OutRespawnDelay)
{
	OutRespawnDelay=RespawnDelay;
	UE_LOG(LogSilvaGame, Log, TEXT("Player has died during duel"))
	auto DeadPC=Cast<AArenaPlayerController>(DeadPlayer->GetController());

	if (DeadPC)
	{
		FDuelPlayerState* State=FindPlayer(DeadPC);
		if (State)
		{
			EndDuel(State->InArena, *FindPlayer(State->Opponent), *State);
		}
	}
	//TODO: set bIsAlive to true or do replace char with fresh copy
	if(auto Player=Cast<AArenaPlayerCharacter>(DeadPC->GetPawn()))
	{
		Player->PlayerStatus=EPlayerState::AliveNonCombat;
	}
}

void ADuelGameMode::InitializeDuels()
{
	UE_LOG(LogSilvaGame, Display, TEXT("Initializing Duel Phase with %i players"), Players.Num())

	switch (Players.Num())
	{
	case 4:
		DuelsLeft=4;
		break;
	case 3:
		DuelsLeft=3;
		break;
	case 2:
		DuelsLeft=1;
		break;
	default:
		DuelsLeft=0;
	}
	
	EvaluateDuels();
}

void ADuelGameMode::HandleWin()
{
	if (Players.Num()>0)
	{
		UE_LOG(LogSilvaGame, Display, TEXT("Player %s has won the game."), *Players[0].Controller->PlayerState->GetPlayerName())
		for (auto& Player : Players)
		{
			FString WinMsg=FString::Printf(TEXT("Player %s has won the game!"),*Players[0].Controller->PlayerState->GetPlayerName());
			Player.Controller->ToggleHUD(true);
			Player.Controller->DisplayMsg(WinMsg);
		}
	}
	if (Players.Num()>1)
	{
		UE_LOG(LogSilvaGame, Display, TEXT("2nd Place: %s."), *Players[1].Controller->PlayerState->GetPlayerName())
	}
	if (Players.Num()>2)
	{
		UE_LOG(LogSilvaGame, Display, TEXT("3rd Place: %s."), *Players[2].Controller->PlayerState->GetPlayerName())
	}
	if (Players.Num()>3)
	{
		UE_LOG(LogSilvaGame, Display, TEXT("Last Place: %s."), *Players[3].Controller->PlayerState->GetPlayerName())
	}
	
	
}

void ADuelGameMode::EvaluateDuels()
{
	Players.Sort([](const FDuelPlayerState& A, const FDuelPlayerState& B){ return A.Wins>B.Wins;});
	UE_LOG(LogSilvaGame, Display, TEXT("Evaluating Duels: %i Duels left"), DuelsLeft)
	if (DuelsLeft==0) //No Duels left -> Winner determined
	{
		HandleWin();
	}
	else if (DuelsLeft>1 && bArenaAInProgress==bArenaBInprogress && Players.Num()%2==0) //Duels can happen simultaneously
	{
		auto A=GetNextAvailablePlayer();
		BeginDuel(EArena::A, *A, *GetNextAvailablePlayer(A));
		auto B=GetNextAvailablePlayer();
		BeginDuel(EArena::B, *B, *GetNextAvailablePlayer(B));
	}
	else //one Duel
	{
		if (Players.Num()==3 && DuelsLeft==2) //losers fight first if 3 players
		{
			Players.Sort([](const FDuelPlayerState& A, const FDuelPlayerState& B){ return A.Wins<B.Wins;});
		}
		auto A=GetNextAvailablePlayer();
		if (!bArenaAInProgress)
		{
			BeginDuel(EArena::A, *A, *GetNextAvailablePlayer(A));
		}
		else
		{
			BeginDuel(EArena::B, *A, *GetNextAvailablePlayer(A));
		}
	}
	
}

void ADuelGameMode::BeginDuel(EArena InArena, FDuelPlayerState& PlayerA, FDuelPlayerState& PlayerB)
{
	switch (InArena)
	{
	case EArena::A:
		PlayerA.Controller->GetPawn()->SetActorTransform(DuelPlayerStarts[0]->GetActorTransform());
		PlayerB.Controller->GetPawn()->SetActorTransform(DuelPlayerStarts[1]->GetActorTransform());
		bArenaAInProgress=true;
		Doors[0]->OpenDoorAfterDelay(PreparationTime);
		Doors[1]->OpenDoorAfterDelay(PreparationTime);
		break;
	case EArena::B:
		PlayerA.Controller->GetPawn()->SetActorTransform(DuelPlayerStarts[2]->GetActorTransform());
		PlayerB.Controller->GetPawn()->SetActorTransform(DuelPlayerStarts[3]->GetActorTransform());
		Doors[2]->OpenDoorAfterDelay(PreparationTime);
		Doors[3]->OpenDoorAfterDelay(PreparationTime);
		bArenaBInprogress=true;
		break;
		default:
			UE_LOG(LogSilvaGame, Error, TEXT("Arena None specified in begin Duel"))
		return;
	}

	PlayerA.bInDuel=true;
	PlayerB.bInDuel=true;
	PlayerA.InArena=InArena;
	PlayerB.InArena=InArena;
	PlayerA.Opponent=PlayerB.Controller;
	PlayerB.Opponent=PlayerA.Controller;
	PlayerA.Controller->ToggleHUD(true);
	PlayerB.Controller->ToggleHUD(true);
	PlayerA.Controller->SetupTimerUI(FTimespan::FromSeconds(PreparationTime));
	PlayerB.Controller->SetupTimerUI(FTimespan::FromSeconds(PreparationTime));
	auto CharA=Cast<AArenaPlayerCharacter>(PlayerA.Controller->GetPawn());
	auto CharB=Cast<AArenaPlayerCharacter>(PlayerB.Controller->GetPawn());
	CharA->PlayerStatus=EPlayerState::Alive;
	CharB->PlayerStatus=EPlayerState::Alive;
}

void ADuelGameMode::EndDuel(EArena InArena, FDuelPlayerState& Winner, FDuelPlayerState& Loser)
{
	
	UE_LOG(LogSilvaGame, Display,TEXT("Duel in Arena %s has ended. %s has won against %s"),
		*UEnum::GetValueAsString(InArena),
		*Winner.Controller->PlayerState->GetPlayerName(),
		*Loser.Controller->PlayerState->GetPlayerName())

	DuelsLeft-=1;
	Winner.bInDuel=false;
	Loser.bInDuel=false;
	Winner.Wins+=1;
	Winner.Opponent=nullptr;
	Loser.Opponent=nullptr;
	Winner.InArena=EArena::None;
	Loser.InArena=EArena::None;
	Winner.Controller->ToggleHUD(false);
	Loser.Controller->ToggleHUD(false);
	//Loser already has player status set differently
	auto CharA=Cast<AArenaPlayerCharacter>(Winner.Controller->GetPawn());
	CharA->PlayerStatus=EPlayerState::AliveNonCombat;
	
	if (auto Player=Cast<AArenaPlayerCharacter>(Winner.Controller->GetPawn()))
	{
		Player->InitializeAttributes();
	}
	if (auto Player=Cast<AArenaPlayerCharacter>(Loser.Controller->GetPawn()))
	{
		Player->InitializeAttributes();
	}
	
	
	switch (InArena)
	{
	case EArena::A:
		Doors[0]->CloseDoor();
		Doors[1]->CloseDoor();
		bArenaAInProgress=false;
		break;

	case EArena::B:
		Doors[2]->CloseDoor();
		Doors[3]->CloseDoor();
		bArenaBInprogress=false;
		break;
	default:
		UE_LOG(LogSilvaGame, Error, TEXT("Arena None specified in begin Duel"))
	}
	if (bArenaAInProgress==false && bArenaBInprogress==false )
	{
		EvaluateDuels();
	}
}

FDuelPlayerState* ADuelGameMode::GetNextAvailablePlayer(FDuelPlayerState* StateToIgnore)
{
	for (auto& Player : Players)
	{
		if (Player.bInDuel==false && &Player!=StateToIgnore)
		{
			return &Player;
		}
	}
	return nullptr;
}

FDuelPlayerState* ADuelGameMode::FindPlayer(AArenaPlayerController* InController)
{
	for (auto& Player : Players)
	{
		if (Player.Controller==InController)
		{
			return &Player;
		}
	}
	return nullptr;
}
