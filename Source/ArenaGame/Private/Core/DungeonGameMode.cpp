// Copyright Runescripters 2021


#include "Core/DungeonGameMode.h"

#include "ArenaGame/ArenaGame.h"
#include "Core/ArenaGameState.h"
#include "Enemies/ArenaEnemyCharacterBase.h"
#include "TileBase.h"
#include "TilemapGenerator.h"
#include "DA_EnemySpawnGroup.h"
#include "OnlineSubsystem.h"
#include "Core/ArenaPlayerCharacter.h"
#include "Core/ArenaPlayerController.h"
#include "Core/ArenaPlayerState.h"
#include "Engine/AssetManager.h"
#include "GameFramework/PlayerState.h"
#include "Gameplay/Chest.h"
#include "GAS/GASAbilitySpell.h"
#include "GAS/GASComponent.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ADungeonGameMode::ADungeonGameMode()
{
	Directions.Emplace(FVector(1, 0, 0));
		Directions.Emplace(FVector(0, 1, 0));
		Directions.Emplace(FVector(-1, 0, 0));
		Directions.Emplace(FVector(0, -1, 0));
}

void ADungeonGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	
	UAssetManager& AssetManager=UAssetManager::Get();
	FPrimaryAssetType SpawnGroups{FName("DA_EnemySpawnGroup")};
	TArray<FPrimaryAssetId> AssetIds;
	AssetManager.GetPrimaryAssetIdList(SpawnGroups, AssetIds);
	LoadSpawnGroupsComplete.BindUObject(this, &ADungeonGameMode::OnLoadSpawnGroupsComplete);
	LoadedGroups=AssetManager.LoadPrimaryAssets(AssetIds, TArray<FName>(), LoadSpawnGroupsComplete);

	TArray<FPrimaryAssetId> RuneIds;
	FPrimaryAssetType Runes{FName("RuneBase")};
	AssetManager.GetPrimaryAssetIdList(Runes, RuneIds);
	LoadedRunes=AssetManager.LoadPrimaryAssets(RuneIds, TArray<FName>(), FStreamableDelegate::CreateUObject(this, &ADungeonGameMode::OnLoadRunesComplete));

	TArray<FPrimaryAssetId> SpellIds;
	FPrimaryAssetType Spells{FName("GASAbilitySpell")};
	AssetManager.GetPrimaryAssetIdList(Spells, SpellIds);
	LoadedSpells=AssetManager.LoadPrimaryAssets(SpellIds, TArray<FName>(), FStreamableDelegate::CreateUObject(this, &ADungeonGameMode::OnLoadSpellsComplete));
	
}

void ADungeonGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	TilemapGenerator=GetWorld()->GetSubsystem<UTilemapGenerator>();
	check(TilemapGenerator);

	FTimerHandle Handle;
	
	if (bAutoStartForDebug)
	{
		
		FString Msg=FString::Printf(TEXT("Debug Begin Play: Autostarting Tile Phase in %f Seconds"), StartWaitTime);
		GEngine->AddOnScreenDebugMessage(87389448, 5.f, FColor::Red, Msg);
		GetWorldTimerManager().SetTimer(Handle, this, &ADungeonGameMode::PrepareDungeonPhase, StartWaitTime, false);
	}
	
}

void ADungeonGameMode::HandlePlayerDeath(ACharacter* DeadPlayer, float& OutRespawnDelay)
{
	OutRespawnDelay=RespawnDelay;
	auto PC=Cast<AArenaPlayerController>(DeadPlayer->GetController());
	auto PlayerCharacter=Cast<AArenaPlayerCharacter>(DeadPlayer);
	ensure(PC);
	ensure(PlayerCharacter);
	
	FGameplayTagContainer PlayerEffectTags;
	PlayerEffectTags.AddTag(FGameplayTag::RequestGameplayTag(FName("CancelOnDeath")));
	PlayerCharacter->AbilitySystemComponent->RemoveActiveEffectsWithTags(PlayerEffectTags);

	FTimerHandle Handle;
	FTimerDelegate Del=FTimerDelegate::CreateUObject(this, &ADungeonGameMode::RespawnPlayer, PC);
	GetWorldTimerManager().SetTimer(Handle, Del, RespawnDelay, false);
	
}

UDA_EnemySpawnGroup* ADungeonGameMode::GetSpawnGroup(ETileType TileType)
{
	TArray<UDA_EnemySpawnGroup*> TargetArray;
	switch (TileType) {
	case ETileType::Easy:
		TargetArray=EasyGroups;
		break;
	case ETileType::Normal:
		TargetArray=NormalGroups;
		break;
	case ETileType::Hard:
		TargetArray=HardGroups;
		break;

		default:
			return nullptr;
	}
	int32 RandomIndex=UKismetMathLibrary::RandomInteger(TargetArray.Num());
	return TargetArray[RandomIndex];
	
}

void ADungeonGameMode::DungeonPhaseEnded()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(this, AArenaPlayerController::StaticClass(), Actors);

	for (auto& Actor : Actors)
	{
		if (auto PC=Cast<AArenaPlayerController>(Actor))
		{
			PC->PrepareInventoryForTravel();
		}
	}

		
	UE_LOG(LogSilvaGame, Log, TEXT("Dungeon Phase has ended"))
	GetGameState<AArenaGameState>()->OnDungeonPhaseEnded.Broadcast();
	FString LevelPath=*FPackageName::ObjectPathToPackageName(ArenaLevel.ToString());
	GetWorld()->ServerTravel(LevelPath);
}

void ADungeonGameMode::RespawnPlayer(AArenaPlayerController* PlayerController)
{
	AArenaPlayerCharacter* DeadPlayer=Cast<AArenaPlayerCharacter>(PlayerController->GetPawn());
	if (!PlayerController || !DeadPlayer)
	{
		UE_LOG(LogSilvaGame, Error, TEXT("Tried to respawn invalid player"))
		return;
	}
	
	int32 Index=PlayerControllers.Find(PlayerController);
	ensure(PlayerControllers.IsValidIndex(Index));
	FVector PlayerSpawnLoc=StarterTiles[Index]->GetActorLocation();
	StarterTiles[Index]->EnterTile(DeadPlayer, EDirection::North);
	PlayerSpawnLoc.X-=300.f;
	DeadPlayer->SetActorLocation(PlayerSpawnLoc);
	
	auto Context=DeadPlayer->AbilitySystemComponent->MakeEffectContext();
	DeadPlayer->AbilitySystemComponent->BP_ApplyGameplayEffectToSelf(DeadPlayer->DefaultAttributeSetterEffect, 1.f, Context);
	DeadPlayer->PlayerStatus=EPlayerState::Alive;
}

void ADungeonGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (auto ArenaPC=Cast<AArenaPlayerController>(NewPlayer))
	{
		PlayerControllers.Add(ArenaPC);
		
		UE_LOG(LogSilvaOnline, Display, TEXT("Logging in Player"))

		if (auto PlayerState=Cast<AArenaPlayerState>(ArenaPC->PlayerState))
		{
			PlayerState->UpdatePlayerIndex(PlayerControllers.Num()-1);
		}
	}
	
	
}

void ADungeonGameMode::PrepareDungeonPhase()
{
	SetupStarterTiles();
	int8 PlayerIndex=0;
	for (auto Direction : Directions)
	{
		if (PlayerControllers.Num() <= PlayerIndex)
		{
			break;
		}
		if (StarterTiles[PlayerIndex])
		{
			StarterTiles[PlayerIndex]->SetActorLocation(Direction*PaddingDistance);
		}
		
		
		FVector PlayerSpawnLoc=StarterTiles[PlayerIndex]->GetActorLocation();
		PlayerSpawnLoc.X-=300.f;
		StarterTiles[PlayerIndex]->EnterTile(PlayerControllers[PlayerIndex]->GetPawnOrSpectator(), EDirection::North);
		PlayerControllers[PlayerIndex]->GetPawn()->SetActorLocationAndRotation(PlayerSpawnLoc, FRotator::ZeroRotator);
		PlayerControllers[PlayerIndex]->SetupTimerUI(DungeonPhaseDuration);
		auto ArenaController=Cast<AArenaPlayerController>(PlayerControllers[PlayerIndex]);
		if (ArenaController)
		{
			ArenaController->ToggleHUD(true);
			if (auto ArenaPlayer=Cast<AArenaPlayerCharacter>(ArenaController->GetPawn()))
			{
				ArenaPlayer->PlayerStatus=EPlayerState::Alive;
			}
		}
		++PlayerIndex;
	}

	
	GetGameState<AArenaGameState>()->OnDungeonPhaseBegin.Broadcast(DungeonPhaseDuration);
	GetWorldTimerManager().SetTimer(DungeonPhaseTimer, this, &ADungeonGameMode::DungeonPhaseEnded, DungeonPhaseDuration.GetTotalSeconds(), false);
	UE_LOG(LogSilvaGame, Log, TEXT("Beginning Dungeon Phase"))
	
}

TSubclassOf<UObject> ADungeonGameMode::GetRandomRuneClass()
{
	int32 RandIndex=FMath::RandRange(0,RuneClasses.Num()-1);
	return RuneClasses[RandIndex];
}

TSubclassOf<UObject>  ADungeonGameMode::GetRandomSpellClass()
{
	int32 RandIndex=FMath::RandRange(0,SpellClasses.Num()-1);
	return SpellClasses[RandIndex];
}

TSubclassOf<AActor> ADungeonGameMode::GetPotionClass()
{
	return HealthPickup;
}

TSubclassOf<AActor> ADungeonGameMode::GetGoldClass()
{
	return GoldPickup;
}


UClass* ADungeonGameMode::GetChestClass(ETileType Difficulty)
{
	switch (Difficulty)
	{
	case ETileType::Easy:
		return EasyChest;
	case ETileType::Normal:
		return MediumChest;
	case ETileType::Hard:
		return HardChest;
	default:
		return nullptr;
	}
}


void ADungeonGameMode::OnLoadSpawnGroupsComplete()
{
	UE_LOG(LogTemp, Warning, TEXT("Load Spawn Groups Complete"));
	if (!ensureMsgf(LoadedGroups.IsValid(), TEXT("Loaded Spawn Groups Ptr is invalid"))) return;

	TArray<UObject*> Assets;
	LoadedGroups->GetLoadedAssets(Assets);
	for (auto Asset : Assets)
	{
		UDA_EnemySpawnGroup* SpawnGroup=Cast<UDA_EnemySpawnGroup>(Asset);
		if (SpawnGroup)
		{
			switch (SpawnGroup->Difficulty)
			{
			case EEnemyDifficulty::Easy:
				EasyGroups.Add(SpawnGroup);
				break;
			case EEnemyDifficulty::Normal:
				NormalGroups.Add(SpawnGroup);
				break;
			case EEnemyDifficulty::Hard:
				HardGroups.Add(SpawnGroup);
				break;
			}
		}
	}
	
}

void ADungeonGameMode::OnLoadRunesComplete()
{
	UE_LOG(LogSilvaGame, Log, TEXT("Loaded Runes"))
	TArray<UObject*> Runes;
	LoadedRunes->GetLoadedAssets(Runes);
	for(auto& Elem: Runes)
	{
		TSubclassOf<UObject> Class=Cast<UClass>(Elem);
		if (!Class) continue;
		RuneClasses.Add(Class);
	}

}

void ADungeonGameMode::OnLoadSpellsComplete()
{
	UE_LOG(LogSilvaGame, Log, TEXT("Loaded Spells"))
	TArray<UObject*> Spells;
	LoadedSpells->GetLoadedAssets(Spells);
	for(auto& Elem: Spells)
	{
		TSubclassOf<UObject> Class=Cast<UClass>(Elem);
		if (!Class) continue;
		SpellClasses.Add(Class);
	}
	
}

void ADungeonGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	AArenaPlayerController* PC=Cast<AArenaPlayerController>(Exiting);
	if (PC)
	{
		PlayerControllers.Remove(PC);
	}
	
}


void ADungeonGameMode::SetupStarterTiles()
{
	//get hold of four starter tiles and set them to the default start distance
	for (int32 PlayerIndex = 0; PlayerIndex < PlayerControllers.Num(); ++PlayerIndex)
	{
		ATileBase* StarterTile = TilemapGenerator->CreateNewTile(nullptr, ETileType::Start, EDirection::North,
			PlayerControllers[PlayerIndex]);
		StarterTiles.Add(StarterTile);
		StarterTile->SetIndex(0, 0);
	}
	
}
