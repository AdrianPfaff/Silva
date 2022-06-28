// Copyright Runescripters 2021


#include "Core/ArenaPlayerState.h"

#include "Core/ArenaGameState.h"
#include "Core/ArenaPlayerCharacter.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

bool AArenaPlayerState::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	Channel->ReplicateSubobjectList(Items, *Bunch, *RepFlags);
	return Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
}

void AArenaPlayerState::RetrieveInventory(TArray<FPlayerSpellSlot>& OutSpells, TArray<UObject*>& OutItems)
{
	OutSpells=SpellSlots;
	OutItems=Items;
}

void AArenaPlayerState::SetInventory(TArray<FPlayerSpellSlot> InSpells, TArray<UObject*> InItems)
{
	SpellSlots=InSpells;
	Items=InItems;
	bShouldReplace=true;
}

void AArenaPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
	if (auto ArenaPlayerState=Cast<AArenaPlayerState>(PlayerState))
	{
		ArenaPlayerState->UpdatePlayerIndex(GetIndex());

		TArray<FPlayerSpellSlot> NewSpells;
		TArray<UObject*> NewItems;
		RetrieveInventory(NewSpells, NewItems);
		ArenaPlayerState->SetInventory(NewSpells, NewItems);
	}
}

void AArenaPlayerState::OnRep_PlayerIndex()
{
	auto Pawn=GetPawn<AArenaPlayerCharacter>();
	auto GS=GetWorld()->GetGameState<AArenaGameState>();
	if (!Pawn || !GS)
	{
		UE_LOG(LogSilvaOnline, Warning, TEXT("Error replicating player index: GameState or Pawn is not valid (yet), retrying after 0.2s"))
		GetWorldTimerManager().SetTimer(RetryHandle, this, &AArenaPlayerState::OnRep_PlayerIndex, 0.2f, false);
		return;
	}

	Pawn->SetSkeletalMesh(GS->Meshes[PlayerIndex]);
}

void AArenaPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AArenaPlayerState, PlayerIndex)
	DOREPLIFETIME(AArenaPlayerState, SpellSlots)
	DOREPLIFETIME(AArenaPlayerState, Items)
	DOREPLIFETIME(AArenaPlayerState, bShouldReplace)
	
}
