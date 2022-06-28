// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnumLibrary.h"
#include "SpawnVolume.h"
#include "ChestSpawnComponent.h"
#include "TileBase.generated.h"

class UTileConnectorComponent;

DECLARE_EVENT_OneParam(ATileBase, FOnEnterTileSignature, AActor* /* EnteringActor */)
DECLARE_EVENT_OneParam(ATileBase ,FOnLeaveTileSignature, AActor* /* LeavingActor */)


UCLASS(Abstract, Blueprintable)
class TILEMAPGEN_API ATileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATileBase();
	
	
	FOnEnterTileSignature OnEnterTile;
	
	FOnLeaveTileSignature OnLeaveTile;

	void LeaveTile();
	void EnterTile(AActor* EnteringActor, EDirection EnteringDirection);

	//returns the active actor, most likely the player or nullptr
	AActor* GetActiveActor() const { return ActiveActor; }
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;

	TPair<int8, int8> GetIndex() const;

	UFUNCTION(Reliable, Server)
	void SetIndex(int8 X, int8 Y);

	const TArray<USpawnVolume*>&  GetSpawnVolumes() const { return SpawnVolumes; }

	UFUNCTION(Server, Reliable)
	void SetTileType(ETileType Type);
	void SetTileType_Implementation(ETileType Type) { TileType=Type; }

	ETileType GetTileType() const { return TileType; }

	UFUNCTION(BlueprintPure, BlueprintCallable)
	void GetTileBounds(FVector& Origin, FVector& Extent) const;
	
	TPair<int8, int8> GetNeighbourIndex(EDirection Direction);

	FVector GetConnectorLocation(EDirection Direction);

	uint32 GetEnemiesLeft() const { return LivingEnemies.Num(); }

	void UnlockPortals();
	UFUNCTION()
	void OnEnemyDeath(ACharacter* DeadCharacter);

	UFUNCTION()
	void OnPlayerDeath();

protected:

	UPROPERTY(BlueprintReadWrite, Replicated, EditDefaultsOnly)
	UChildActorComponent* Shop;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Base Mesh that acts as the horizontal bounds of each Tile
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BasePlate;

	//Wall Mesh for Tiles
	UPROPERTY()
	UStaticMeshComponent* NorthWall;
	UPROPERTY()
	UStaticMeshComponent* EastWall;
	UPROPERTY()
	UStaticMeshComponent* SouthWall;
	UPROPERTY()
	UStaticMeshComponent* WestWall;

	//The four connectors to the neighbour tiles
	UPROPERTY()
	UTileConnectorComponent* NorthConnector;
	UPROPERTY()
	UTileConnectorComponent* EastConnector;
	UPROPERTY()
	UTileConnectorComponent* SouthConnector;
	UPROPERTY()
	UTileConnectorComponent* WestConnector;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BlockingVolume;

	UPROPERTY(BlueprintReadOnly, Replicated)
	ETileType TileType;

	//The active actor, most likely either the player or nullptr
	UPROPERTY()
	AActor* ActiveActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UChestSpawnComponent* ChestSpawn;

	UPROPERTY()
	TArray<ACharacter*> LivingEnemies;
	
	private:

	void SetupWallRotation();
	
	void SetupConnectors();
	
	TPair<int8, int8> Index;

	UPROPERTY()
	TArray<USpawnVolume*> SpawnVolumes;

	void SpawnChest();

	bool bHasBeenCleared=false;
	
};
