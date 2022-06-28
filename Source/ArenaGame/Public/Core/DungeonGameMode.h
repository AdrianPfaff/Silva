// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "GameFramework/GameModeBase.h"
#include "EnumLibrary.h"
#include "GameModeInterface.h"

#include "DungeonGameMode.generated.h"

/**
 * 
 */

class AChest;
class UDA_EnemySpawnGroup;
class AArenaPlayerController;
class AArenaPlayerCharacter;

UCLASS()
class ARENAGAME_API ADungeonGameMode : public AGameModeBase, public IGameModeInterface
{
	GENERATED_BODY()

public:
	ADungeonGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void BeginPlay() override;

	

	virtual void HandlePlayerDeath(ACharacter* DeadPlayer, float& OutRespawnDelay) override;

	virtual UDA_EnemySpawnGroup* GetSpawnGroup(ETileType TileType) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	UFUNCTION(BlueprintCallable)
	void PrepareDungeonPhase();

	UFUNCTION(BlueprintPure)
	TSubclassOf<UObject> GetRandomRuneClass();
	UFUNCTION(BlueprintPure)
	TSubclassOf<UObject> GetRandomSpellClass();
	UFUNCTION(BlueprintPure)
	TSubclassOf<AActor> GetPotionClass();
	
	TSubclassOf<AActor> GetGoldClass();

	virtual UClass* GetChestClass(ETileType Difficulty) override;
	
protected:

	uint8 PlayerIndices=0;
	
	//The distance in UUnits from the World Origin from which the starter tiles will be placed. Defaults to 1km.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PaddingDistance = 100000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bAutoStartForDebug;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTimespan DungeonPhaseDuration=FTimespan(0, 5, 0);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UWorld> ArenaLevel;
	
	//Generates a starter tile for each player and sets it up according the properties above 
	virtual void SetupStarterTiles();

	//cached pointer to the tilemap gen subsystem
	UPROPERTY(BlueprintReadOnly)
	class UTilemapGenerator* TilemapGenerator;

	UPROPERTY(BlueprintReadOnly)
	TArray<AArenaPlayerController*> PlayerControllers;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float RespawnDelay=5.f;

	UFUNCTION()
	void OnLoadSpawnGroupsComplete();

	UFUNCTION()
	void OnLoadRunesComplete();

	UFUNCTION()
	void OnLoadSpellsComplete();

	UFUNCTION()
	void DungeonPhaseEnded();

	UFUNCTION()
	void RespawnPlayer(AArenaPlayerController* PlayerController);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float StartWaitTime=60.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AChest> EasyChest;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AChest> MediumChest;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AChest> HardChest;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> HealthPickup;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> GoldPickup;
	
	UPROPERTY(BlueprintReadOnly)
	TArray<TSubclassOf<UObject>> RuneClasses;

	UPROPERTY(BlueprintReadOnly)
	TArray<TSubclassOf<UObject>> SpellClasses;
	
private:
	UPROPERTY()
	TArray<class ATileBase*> StarterTiles;

	TArray<FVector> Directions;

	FStreamableDelegate LoadSpawnGroupsComplete;
	TSharedPtr<FStreamableHandle> LoadedGroups;

	
	TSharedPtr<FStreamableHandle> LoadedSpells;
	
	
	TSharedPtr<FStreamableHandle> LoadedRunes;

	UPROPERTY()
	TArray<class UDA_EnemySpawnGroup*> EasyGroups;
	UPROPERTY()
	TArray<UDA_EnemySpawnGroup*> NormalGroups;
	UPROPERTY()
	TArray<UDA_EnemySpawnGroup*> HardGroups;

	FTimerHandle DungeonPhaseTimer;
	
	
};
