// Copyright Runescripters 2021


#include "TileBase.h"

#include "GameModeInterface.h"
#include "PlayerTileSetter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "Net/UnrealNetwork.h"
#include "SpawnVolume.h"
#include "TileConnectorComponent.h"
#include "TilemapGenerator.h"
#include "GameFramework/Character.h"

// Sets default values
ATileBase::ATileBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	//Base Plate: 
	BasePlate=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Plate"));
	RootComponent=BasePlate;

	BlockingVolume=CreateDefaultSubobject<UBoxComponent>(TEXT("Blocking Volume"));
	BlockingVolume->SetupAttachment(RootComponent);
	BlockingVolume->SetCollisionProfileName(FName("InvisibleWallDynamic"));
	BlockingVolume->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Block);
	

	BlockingVolume->SetBoxExtent(FVector(1250.f, 1250.f, 100.f));
	BlockingVolume->SetRelativeLocation(FVector(0.f, 0.f, 780.f));
	
	auto PlateMesh=ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Arena/Assets/SharedTileAssets/SM_TileGround.SM_TileGround'"));
	check(PlateMesh.Succeeded());
		if (PlateMesh.Succeeded())
		{
			BasePlate->SetStaticMesh(PlateMesh.Object);
		}
	//Tile Walls:
	NorthWall=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("North Wall"));
	NorthWall->SetupAttachment(RootComponent);
	EastWall=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("East Wall"));
	EastWall->SetupAttachment(RootComponent);
	SouthWall=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("South Wall"));
	SouthWall->SetupAttachment(RootComponent);
	WestWall=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("West Wall"));
	WestWall->SetupAttachment(RootComponent);

	auto WallMesh=ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Arena/Assets/SharedTileAssets/SM_DungeonWall.SM_DungeonWall'"));
	check(WallMesh.Succeeded());
	if (WallMesh.Succeeded())
	{
		NorthWall->SetStaticMesh(WallMesh.Object);
		EastWall->SetStaticMesh(WallMesh.Object);
		SouthWall->SetStaticMesh(WallMesh.Object);
		WestWall->SetStaticMesh(WallMesh.Object);
	}

	NorthWall->AddRelativeLocation(FVector(0, 0, -10.f));
	EastWall->AddRelativeLocation(FVector(0, 0, -10.f));
	SouthWall->AddRelativeLocation(FVector(0, 0, -10.f));
	WestWall->AddRelativeLocation(FVector(0, 0, -10.f));
	
	//Tile Connectors

	NorthConnector=CreateDefaultSubobject<UTileConnectorComponent>(TEXT("North Connector"));
	NorthConnector->SetupAttachment(RootComponent);
	EastConnector=CreateDefaultSubobject<UTileConnectorComponent>(TEXT("East Connector"));
	EastConnector->SetupAttachment(RootComponent);
	SouthConnector=CreateDefaultSubobject<UTileConnectorComponent>(TEXT("Sout Connector"));
	SouthConnector->SetupAttachment(RootComponent);
	WestConnector=CreateDefaultSubobject<UTileConnectorComponent>(TEXT("West Connector"));
	WestConnector->SetupAttachment(RootComponent);

	ChestSpawn=CreateDefaultSubobject<UChestSpawnComponent>(TEXT("Chest Spawn"));
	ChestSpawn->SetupAttachment(RootComponent);
	
	
	bReplicates=true;
	bOnlyRelevantToOwner=true;
}

void ATileBase::GetTileBounds(FVector& Origin, FVector& Extent) const
{
	FBoxSphereBounds Bounds = BasePlate->Bounds;
	Origin = Bounds.Origin;
	Extent = Bounds.BoxExtent;
}

TPair<int8, int8> ATileBase::GetNeighbourIndex(EDirection Direction)
{
	TPair<int8, int8> NeighbourIndex=Index;
	switch (Direction) {
		case EDirection::North:
			NeighbourIndex.Key-=1;
			break;
		case EDirection::East:
			NeighbourIndex.Value+=1;
			break;
		case EDirection::South:
			NeighbourIndex.Key+=1;
			break;
		case EDirection::West:
			NeighbourIndex.Value-=1;
			break;
		
	}
	return NeighbourIndex;
}

FVector ATileBase::GetConnectorLocation(EDirection Direction)
{
	switch (Direction)
	{
	case EDirection::North:
		return NorthConnector->GetComponentLocation();
	case EDirection::East:
		return EastConnector->GetComponentLocation();
	case EDirection::South:
		return SouthConnector->GetComponentLocation();
	case EDirection::West:
		return WestConnector->GetComponentLocation();
	}

	return FVector::ZeroVector;
}

void ATileBase::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATileBase , TileType);
	DOREPLIFETIME(ATileBase, Shop);
}

void ATileBase::UnlockPortals()
{
	NorthConnector->UnlockPortal();
	SouthConnector->UnlockPortal();
	EastConnector->UnlockPortal();
	WestConnector->UnlockPortal();
}

void ATileBase::OnEnemyDeath(ACharacter* DeadCharacter)
{
	LivingEnemies.Remove(DeadCharacter);
	if (LivingEnemies.Num()==0)
	{
		bHasBeenCleared=true;
		SpawnChest();
		UnlockPortals();
	}
}

void ATileBase::OnPlayerDeath()
{
	//if enemies are left, destroy them
	if (LivingEnemies.Num()>0)
	{
		for (auto& Enemy : LivingEnemies)
		{
			Enemy->Destroy();
		}
		LivingEnemies.Empty();
	}
	
}

// Called when the game starts or when spawned
void ATileBase::BeginPlay()
{
	Super::BeginPlay();
	BasePlate->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECR_Block);
	SetupConnectors();
	SetupWallRotation();

#if UE_BUILD_DEVELOPMENT
	FString Msg=UEnum::GetValueAsString(TileType);
	GEngine->AddOnScreenDebugMessage(48358405, 3.f, FColor::Red, Msg);
#endif
	
}

void ATileBase::SetupWallRotation()
{
	
	NorthWall->AddRelativeRotation(FRotator(0,90, 0));
	EastWall->AddRelativeRotation(FRotator(0,180,0));
	SouthWall->AddRelativeRotation(FRotator(0,270,0));
	
}


void ATileBase::SetupConnectors()
{
	FVector ActorBounds=RootComponent->Bounds.BoxExtent;;

	FVector NorthLoc=FVector::ZeroVector;
	NorthLoc.X=ActorBounds.X - 150.f;
	NorthLoc.Z-=10.f;
	NorthConnector->SetRelativeLocation(NorthLoc);
	NorthConnector->SetDirection(EDirection::North);
	
	FVector EastLoc=FVector::ZeroVector;
	EastLoc.Y=ActorBounds.Y - 150.f;
	EastLoc.Z-=10.f;
	EastConnector->SetRelativeLocation(EastLoc);
	EastConnector->SetDirection(EDirection::East);

	FVector SouthLoc=FVector::ZeroVector;
	SouthLoc.X=-ActorBounds.X + 150.f;
	SouthLoc.Z-=10.f;
	SouthConnector->SetRelativeLocation(SouthLoc);
	SouthConnector->SetDirection(EDirection::South);

	FVector WestLoc=FVector::ZeroVector;
	WestLoc.Y=-ActorBounds.Y + 150.f;
	WestLoc.Z-=10.f;
	WestConnector->SetRelativeLocation(WestLoc);
	WestConnector->SetDirection(EDirection::West);
}

void ATileBase::SpawnChest()
{
	
	if (auto GameMode=Cast<IGameModeInterface>(GetWorld()->GetAuthGameMode()))
	{
		FTransform ChestTransform=ChestSpawn->GetComponentTransform();
		UClass* ChestType=GameMode->GetChestClass(TileType);

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor(ChestType, &ChestTransform, Params);
	}
	
}


void ATileBase::LeaveTile()
{
	OnLeaveTile.Broadcast(ActiveActor);
	ActiveActor=nullptr;
	
}

void ATileBase::EnterTile(AActor* EnteringActor, EDirection EnteringDirection)
{
	if (Shop && Shop->GetChildActor())
	{
		Shop->GetChildActor()->SetOwner(EnteringActor);
	}
	ActiveActor=EnteringActor;
	UTilemapGenerator* Generator=GetWorld()->GetSubsystem<UTilemapGenerator>();
	auto GameMode=GetWorld()->GetAuthGameMode();
	IGameModeInterface* GroupGetter=Cast<IGameModeInterface>(GameMode);
	if (TileType==ETileType::Shop || TileType==ETileType::Start)
	{
		UnlockPortals();
	}
	else if (Generator && GroupGetter)
	{
		if (!bHasBeenCleared && LivingEnemies.Num()==0)
		{
			Generator->SpawnEnemiesOnTile(this, EnteringDirection, GroupGetter->GetSpawnGroup(TileType), LivingEnemies);
		}
		if (auto Setter=Cast<IPlayerTileSetter>(EnteringActor))
		{
			Setter->SetCurrentTile(this);
		}
	}
	OnEnterTile.Broadcast(EnteringActor);

	//Note: Entering Direction is the inverse of what we need to offset by
	FVector Offset=FVector::ZeroVector;
	switch (EnteringDirection)
	{
	case EDirection::North:
		Offset=FVector(-100.f, 0.f, 0.f);
		break;
	case EDirection::East:
		Offset=FVector(0.f, -100.f, 0.f);
		break;
	case EDirection::South:
		Offset=FVector(100.f, 0.f, 0.f);
	 break;
	case EDirection::West:
		Offset=FVector(0.f, 100.f, 0.f);
		break;
	}
	if (EnteringActor)
	{
		EnteringActor->SetActorLocation(GetConnectorLocation(EnteringDirection)+Offset);
	}
	
}

// Called every frame
void ATileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	for (auto Component : GetComponents())
	{
		auto SpawnVolume=Cast<USpawnVolume>(Component);
		if (SpawnVolume)
		{
			SpawnVolumes.Add(SpawnVolume);
		}
	}
}

TPair<int8, int8> ATileBase::GetIndex() const
{
	return Index;
}

void ATileBase::SetIndex_Implementation(int8 X, int8 Y)
{
	Index.Key=X;
	Index.Value=Y;
}

