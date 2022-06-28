// Copyright Runescripters 2021


#include "Core/ArenaPlayerController.h"

#include "ArenaGame/ArenaGame.h"
#include "Core/ArenaPlayerCharacter.h"
#include "Engine/ActorChannel.h"
#include "GameFramework/PlayerInput.h"
#include "Net/UnrealNetwork.h"
#include "Core/ArenaGameState.h"
#include "Core/ArenaPlayerState.h"
#include "Core/DungeonGameMode.h"

AArenaPlayerController::AArenaPlayerController()
{
	bReplicates=true;
}

void AArenaPlayerController::UpdateSpellIcons_Implementation()
{
	if (HUD)
	{
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([this](){ if(HUD){HUD->UpdateSpellData();}}), 0.1f, false);
	}
}

void AArenaPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
}

void AArenaPlayerController::GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList)
{
	Super::GetSeamlessTravelActorList(bToEntry, ActorList);
	ActorList.Add(GetPawn());
}

void AArenaPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetupUI();

	if (auto PlayerCharacter=Cast<AArenaPlayerCharacter>(GetPawn()))
	{
		
		PlayerCharacter->OnAbilityCommited.AddDynamic(this, &AArenaPlayerController::SetAbilityCooldownInUI);
		UE_LOG(LogSilvaUI, Display, TEXT("Binding Ability Trigger notify to update UI cooldown"))
	}
	
}

void AArenaPlayerController::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();
}

void AArenaPlayerController::PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel)
{
	if (Inventory)
	{
		Inventory->RemoveFromViewport();
		
	}

	if (HUD)
	{
		HUD->RemoveFromViewport();
	}
	
	Super::PreClientTravel(PendingURL, TravelType, bIsSeamlessTravel);
}

void AArenaPlayerController::SetupUI()
{
	//only add HUD to local Player, not Server instance
	if (IsLocalPlayerController())
	{
		HUD=CreateWidget<UNavWidget_HUD>(this, HUDClass);
		if (!ensure(HUD))
		{
			UE_LOG(LogSilvaUI, Error, TEXT("Failed to Create HUD"))
			
		}
		Inventory=CreateWidget<UNavWidget_Inventory>(this, InventoryClass);
		if (!ensure(Inventory))
		{
			UE_LOG(LogSilvaUI, Error, TEXT("Failed to Create Inventory"))
		}
	}
}


void AArenaPlayerController::SetupTimerUI_Implementation(const FTimespan& InTime)
{
	if (HUD)
	{
		HUD->SetupTimer(InTime);
	}
}

void AArenaPlayerController::DisplayMsg_Implementation(const FString& Msg)
{
	if (HUD)
	{
		HUD->DisplayMessage(Msg);
	}
}

void AArenaPlayerController::OpenInventory()
{
	auto PlayerChar=Cast<AArenaPlayerCharacter>(GetPawn());
	if (!PlayerChar || PlayerChar->PlayerStatus!=EPlayerState::Alive)
	{
		return;
	}
		UE_LOG(LogSilvaUI, Log, TEXT("Inventory added to viewport"))
		PlayerInput->FlushPressedKeys();
		Inventory->AddToViewport();
		bShowMouseCursor=true;
		SetInputMode(FInputModeUIOnly());
}

void AArenaPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (IsLocalController())
	{
		InputComponent->BindAction("ToggleInventory", EInputEvent::IE_Pressed, this, &AArenaPlayerController::OpenInventory);
	}
	
}

void AArenaPlayerController::PrepareInventoryForTravel()
{
	auto PlayerChar=Cast<AArenaPlayerCharacter>(GetPawn());
	auto ArenaPlayerState=GetPlayerState<AArenaPlayerState>();
	if (PlayerChar && ArenaPlayerState)
	{
		UE_LOG(LogSilvaGame, Display, TEXT("Copying inventory from PlayerChar to PlayerState for seamless travel"))

		TArray<FPlayerSpellSlot> Spells;
		TArray<UObject*> Items;
		PlayerChar->GetInventoryComponent()->CopyInventory(Spells, Items);
		ArenaPlayerState->SetInventory(Spells, Items);
		bAfterSeamlessTravel=true;
	}
}

void AArenaPlayerController::SetAbilityCooldownInUI_Implementation(EGASAbilityInputID InputID, float Cooldown)
{
	if (HUD)
	{
		HUD->AbilityTriggered(InputID, Cooldown);
	}
	
}


void AArenaPlayerController::ToggleHUD_Implementation(bool bVisible)
{
	if (!HUD)
	{
		UE_LOG(LogSilvaUI, Error, TEXT("HUD Widget is invalid"))
		return;
	}
	
	if (bVisible)
	{
		HUD->AddToViewport(-1);
		UpdateSpellIcons();
	}
	else
	{
		HUD->RemoveFromViewport();
	}
}

