// Copyright Runescripters 2021


#include "TilemapGenerator.h"

#include "DA_EnemySpawnGroup.h"
#include "DeathResponseInterface.h"
#include "EngineUtils.h"
#include "PlaceableTileBase.h"
#include "SubclassTableRow.h"
#include "TileBase.h"
#include "GameFramework/Character.h"
#include "Engine/AssetManager.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TilemapGenerator/TilemapGen.h"


UTilemapGenerator::UTilemapGenerator()
{
	//hardcoded starter tile class
	ConstructorHelpers::FClassFinder<ATileBase> StarterTileFindResult(TEXT("/Game/Arena/StarterTiles/BP_StarterTile"));
	checkf(StarterTileFindResult.Succeeded(), TEXT("Invalid Starter Tile Class"))
	StarterTileClass=StarterTileFindResult.Class;
	
}

void UTilemapGenerator::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	//load Datatables and populate vars
	const auto TilesetAsset =LoadObject<UDataTable>(GetTransientPackage(), TEXT("DataTable'/Game/Arena/TilemapGen/DataTables/DT_Tiles.DT_Tiles'"));
	const auto ShopsAsset =LoadObject<UDataTable>(GetTransientPackage(), TEXT("DataTable'/Game/Arena/TilemapGen/DataTables/DT_Shops.DT_Shops'"));
	
	checkf(TilesetAsset && ShopsAsset, TEXT("Failed to load an Data Table required for the Tilemap Generator"))
	checkf(HardTileDistance>=NormalTileDistance, TEXT("Normal Tile Distance exceeds Hard Tile Distance"))

	Tileset=TilesetAsset;
	Shops=ShopsAsset;
}

void UTilemapGenerator::SpawnEnemiesOnTile(ATileBase* OnTile, EDirection EnteringDirection, UDA_EnemySpawnGroup* SpawnGroup, TArray<ACharacter*>& OutEnemies)
{
	if (!SpawnGroup)
	{
		return;
	}
	FVector ComparisonPoint=OnTile->GetConnectorLocation(EnteringDirection);

	auto SpawnVolumes=OnTile->GetSpawnVolumes();
	if (SpawnVolumes.Num()==0)
	{
		UE_LOG(LogTemp, Error, TEXT("No Spawn Volumes on Tile"))
		return;
	}
	//determine near spawn
	//determine far spawn
	USpawnVolume* NearSpawn=SpawnVolumes[0];
	USpawnVolume* FarSpawn=SpawnVolumes[0];
	for (auto Volume : SpawnVolumes)
	{
		float DistOldNear=UKismetMathLibrary::Vector_DistanceSquared(ComparisonPoint, NearSpawn->GetComponentLocation());
		float DistOldFar=UKismetMathLibrary::Vector_DistanceSquared(ComparisonPoint, FarSpawn->GetComponentLocation());
		float DistNext=UKismetMathLibrary::Vector_DistanceSquared(ComparisonPoint, Volume->GetComponentLocation());
		if (DistNext<DistOldNear)
		{
			NearSpawn=Volume;
			continue;
		}
		if (DistNext>DistOldFar)
		{
			FarSpawn=Volume;
		}
	}
	
	
	FVector Extent{200.f, 200.f, 200.f};
	
	for (auto Bundle : SpawnGroup->EnemyBundles)
	{
		USpawnVolume* TargetVolume=NearSpawn;
		switch(Bundle.SpawnType)
		{
		case EEnemySpawnType::Near:
			TargetVolume=NearSpawn;
			break;
		case EEnemySpawnType::Far:
			TargetVolume=FarSpawn;
			break;
		}
		for (auto& EnemyClassPtr : Bundle.Enemies)
		{
			UE_LOG(LogTemp, Warning, TEXT("Beginning Enemy Spawn from Bundle"))
			FVector RandomPoint=UKismetMathLibrary::RandomPointInBoundingBox(TargetVolume->GetComponentLocation(), TargetVolume->GetScaledBoxExtent());
			
			//FNavLocation Out;
			//GetWorld()->GetNavigationSystem()->GetMainNavData()->ProjectPoint(RandomPoint, Out, Extent);
			auto EnemyClass=EnemyClassPtr.LoadSynchronous();
			AActor* NewEnemy=UGameplayStatics::BeginDeferredActorSpawnFromClass(
				this, EnemyClass, FTransform(RandomPoint), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
			if (auto Enemy=Cast<IDeathResponseInterface>(NewEnemy))
			{
				Enemy->OnDeath.AddDynamic(OnTile, &ATileBase::OnEnemyDeath);
			}
			OutEnemies.Add(Cast<ACharacter>(NewEnemy));
			UGameplayStatics::FinishSpawningActor(NewEnemy, FTransform(RandomPoint));
		}
	}
}

ATileBase* UTilemapGenerator::CreateNewTile(ATileBase* PreviousTile, ETileType Type, EDirection Direction, AActor* Owner)
{
	//different tile types need different handling, allows for future extension
	switch (Type) {
		case ETileType::Start:
			return SpawnStarterTile(Owner);
		
		case ETileType::Shop:
			return SpawnTileFromTable(PreviousTile, Direction, Type, Owner, Shops);
			
		case ETileType::Easy:
		case ETileType::Normal:
		case ETileType::Hard:
			return SpawnTileFromTable(PreviousTile, Direction, Type, Owner, Tileset);
	
		default:
			return nullptr;
	}
}

ATileBase* UTilemapGenerator::CreateNewTileFromLoadedObject(TSubclassOf<APlaceableTileBase> TileClass, ATileBase* PreviousTile, ETileType Type,
	EDirection Direction, AActor* Owner)
{
	FVector SpawnVector=CalculateNextTileLocation(PreviousTile, Direction);
	FTransform SpawnTransform{SpawnVector};
	
	AActor* NewActor=UGameplayStatics::BeginDeferredActorSpawnFromClass(this, TileClass, SpawnTransform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, Owner);
	ATileBase* NewTile= Cast<ATileBase>(NewActor);
	//defer begin play until setting necessary vars
	if (NewTile)
	{
		NewTile->SetTileType(Type);
		TPair<int8, int8> Index=PreviousTile->GetNeighbourIndex(Direction);
		NewTile->SetIndex(Index.Key, Index.Value);
	}
	
	UGameplayStatics::FinishSpawningActor(NewActor, SpawnTransform);
	return NewTile;
}

TSharedPtr<FStreamableHandle> UTilemapGenerator::AsyncRequestTileClass(FStreamableDelegate& FinishDelegate)
{
	//select random row
	TArray<FName> Rows=Tileset->GetRowNames();
	const int32 RandomIndex = UKismetMathLibrary::RandomInteger(Rows.Num());
	FSubclassTableRow* FoundRow=Tileset->FindRow<FSubclassTableRow>(Rows[RandomIndex], "");
	
	if (FoundRow)
	{
		return AsyncLoadClass(FoundRow->Class.ToSoftObjectPath(), FinishDelegate);
	}

	UE_LOG(LogTilemapGen, Error, TEXT("Unable to get an class from Tile Table. Make sure it is set up correctly."))
	return nullptr;
}

TSharedPtr<FStreamableHandle> UTilemapGenerator::AsyncLoadClass(FSoftObjectPath ObjectPath,
                                                                FStreamableDelegate& FinishDelegate)
{
	FStreamableManager& Streamer=UAssetManager::GetStreamableManager();
	return Streamer.RequestAsyncLoad(ObjectPath, FinishDelegate);
}

UClass* UTilemapGenerator::GetRandomClassFromDataTable(const UDataTable* InDataTable) const
{
	//select random row
	TArray<FName> Rows=InDataTable->GetRowNames();
	const int32 RandomIndex = UKismetMathLibrary::RandomInteger(Rows.Num());
	FSubclassTableRow* FoundRow=InDataTable->FindRow<FSubclassTableRow>(Rows[RandomIndex], "");

	if (!FoundRow) return nullptr;
	//load object, potential candidate for async loading
	
	UClass* Class=FoundRow->Class.LoadSynchronous();
	return Class;
		
}

FVector UTilemapGenerator::CalculateNextTileLocation(ATileBase* PreviousTile, EDirection Direction) const
{
	FVector SpawnVector=FVector::ZeroVector;
	if (PreviousTile)
	{
		SpawnVector=PreviousTile->GetActorLocation();
		FVector Origin;
		FVector Extent;
		PreviousTile->GetTileBounds(Origin, Extent);
		//double extent to offset by a whole tile
		Extent*=2;
		
		FVector Offset=FVector::ZeroVector;
		switch (Direction)
		{
		case EDirection::North:
			Offset=FVector(TilePadding+Extent.X, 0.f, 0.f);
			break;
		case EDirection::East:
			Offset=FVector(0.f, TilePadding+Extent.Y, 0.f);
			break;
		case EDirection::South:
			Offset=FVector(-TilePadding-Extent.X, 0.f, 0.f);
			break;
		case EDirection::West:
			Offset=FVector(0.f, -TilePadding-Extent.Y, 0.f);
			break;
		}
		SpawnVector+=Offset;
	}
		return SpawnVector;
}

ATileBase* UTilemapGenerator::SpawnStarterTile(AActor* Owner) const
{
	FActorSpawnParameters Params;
	Params.Owner=Owner;
	AActor* NewTile=GetWorld()->SpawnActor(StarterTileClass, nullptr, nullptr, Params);
	return Cast<ATileBase>(NewTile);
}

ATileBase* UTilemapGenerator::SpawnTileFromTable(ATileBase* PreviousTile, EDirection Direction, ETileType Type,
	AActor* Owner, const UDataTable* DataTable) const
{
	
	if (!ensureMsgf(PreviousTile, TEXT("Previous tile invalid. Can't spawn tile"))) return nullptr;
	
	FVector SpawnVector=CalculateNextTileLocation(PreviousTile, Direction);
	FTransform SpawnTransform{SpawnVector};

	UClass* TileClass=GetRandomClassFromDataTable(DataTable);
	AActor* NewActor=UGameplayStatics::BeginDeferredActorSpawnFromClass(this, TileClass, SpawnTransform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, Owner);
	ATileBase* NewTile= Cast<ATileBase>(NewActor);

	//defer begin play until setting necessary vars
	if (NewTile)
	{
		NewTile->SetTileType(Type);
		TPair<int8, int8> Index=PreviousTile->GetNeighbourIndex(Direction);
		NewTile->SetIndex(Index.Key, Index.Value);
	}
	
	UGameplayStatics::FinishSpawningActor(NewActor, SpawnTransform);
	return NewTile;
}


