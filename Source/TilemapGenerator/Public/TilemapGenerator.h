// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EnumLibrary.h"
#include "PlaceableTileBase.h"
#include "Engine/StreamableManager.h"
#include "TilemapGenerator.generated.h"

class ATileBase;
class UDataTable;
class UDA_EnemySpawnGroup;

/**
 * World Subsystem used to generate Tiles based on a number of rules. Globally accessible by calling
 * GetWorld->GetSubsystem<UTilemapGenerator>() or in Blueprints using Nodes.
 */
UCLASS()
class TILEMAPGEN_API UTilemapGenerator : public UWorldSubsystem
{
	GENERATED_BODY()

	public:
	UTilemapGenerator();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintPure)
	uint8 GetNormalTileDistance() const { return NormalTileDistance; }

	UFUNCTION(BlueprintPure)
	uint8 GetHardTileDistance() const { return HardTileDistance; }

	UFUNCTION()
	void SpawnEnemiesOnTile(class ATileBase* OnTile, EDirection EnteringDirection, UDA_EnemySpawnGroup* SpawnGroup, TArray<ACharacter*>& OutEnemies);

	/**
	 * @brief Generates a new Tile synchronously. Preferred to use CreateNewTileFromLoadedClass instead
	 * @warning PreviousTile can only be nullptr for the starter tiles, otherwise returns nullptr 
	 * @param PreviousTile The Previous tile the new tile should be aligned to, will spawn at 0 0 0 if nullptr
	 * @param Type What type the new tile should be
	 * @param Direction which direction from the previous tile should the new one be created?
	 * @return Tile generated
	 */
	UFUNCTION(BlueprintCallable /* , meta=(DeprecatedFunction)*/ )
	ATileBase* CreateNewTile(ATileBase* PreviousTile, ETileType Type, EDirection Direction, AActor* Owner);

	/**
	 * @brief Generates a new Tile from preloaded class
	 * @warning PreviousTile can only be nullptr for the starter tiles, otherwise returns nullptr 
	 * @param PreviousTile The Previous tile the new tile should be aligned to, will spawn at 0 0 0 if nullptr
	 * @param Type What type the new tile should be
	 * @param Direction which direction from the previous tile should the new one be created?
	 * @return Tile generated
	 */
	UFUNCTION(BlueprintCallable)
	ATileBase* CreateNewTileFromLoadedObject(TSubclassOf<APlaceableTileBase> TileClass, ATileBase* PreviousTile,
	                                              ETileType Type, EDirection Direction, AActor* Owner);

	/**
	 * @brief Asynchronously loads a class and calls back on a delegate
	 * @param FinishDelegate Delegate to call when done
	 * @return Handle to the requested class asset, used to unwrap asset once done loading
	 */
	TSharedPtr<FStreamableHandle> AsyncRequestTileClass(FStreamableDelegate& FinishDelegate);
	
	
	/**
	 * @returns the padding between two tiles in UUnits
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetTilePadding() const { return TilePadding; }
	
	protected:
	//Table of tiles to be used in the generator
	UPROPERTY(BlueprintReadOnly)
	UDataTable* Tileset;

	//Table of Shop variations
	UPROPERTY(BlueprintReadOnly)
	UDataTable* Shops;
	

	UPROPERTY(BlueprintReadOnly)
	UClass* StarterTileClass;

	//How much space should be left between two tiles in UUnits
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly);
	float TilePadding=300.f;

	//index from origin where tiles will be normal difficulty, e.g. 3 means tile 0 4 will be of normal difficulty
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	uint8 NormalTileDistance = 3;
	//index from origin where tiles will be normal difficulty, e.g. 6 means tile 0 7 will be of normal difficulty
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	uint8 HardTileDistance = 6;
	/**
	 * @brief Randomly gets a Class from an Data Table containing Blueprints
	 * @warning requires the Data Table to use the FSubclassTableRow as a base
	 * @param InDataTable Data Table from which to Pull Class
	 * @return Class found
	 */
	UClass* GetRandomClassFromDataTable(const UDataTable* InDataTable) const;

	/**
	 * @brief Helper Function to calculate where the next tile has to be placed in relation to the previous one
	 * @param PreviousTile The previous tile the new one should be attached to. Can be nullptr
	 * @param Direction The direction based on the previous tile the new transform should be set to
	 * @return Transform for the new Tile. Returns Identity Transform if PreviousTile is invalid
	 */
	FVector CalculateNextTileLocation(ATileBase* PreviousTile, EDirection Direction) const;

	/**
	 * @brief Asynchronously loads a class and calls back on a delegate
	 * @param ObjectPath Class to Load
	 * @param FinishDelegate Delegate to call when done
	 * @return Handle to the requested class asset, used to unwrap asset once done loading
	 */
	TSharedPtr<FStreamableHandle> AsyncLoadClass(FSoftObjectPath ObjectPath, FStreamableDelegate& FinishDelegate);

	
	/**
	 * @brief makes a new starter tile from a hard-coded class
	 * @return new starter tile spawned
	 */
	ATileBase* SpawnStarterTile(AActor* Owner) const;
	
	/**
	 * @brief 
	 * @param PreviousTile the previous tile the new tile should be attached to
	 * @param Direction the direction from the previous tile where the new one should be attached to
	 * @return the new tile spawned
	 */
	ATileBase* SpawnTileFromTable(ATileBase* PreviousTile, EDirection Direction, ETileType Type, AActor* Owner, const UDataTable* DataTable) const;

	
};
