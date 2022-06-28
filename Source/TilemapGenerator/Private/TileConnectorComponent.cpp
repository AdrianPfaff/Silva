// Copyright Runescripters 2021


#include "TileConnectorComponent.h"

#include "DrawDebugHelpers.h"
#include "PlaceableTileBase.h"
#include "TileBase.h"
#include "TilemapGenerator.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TilemapGenerator/TilemapGen.h"

// Sets default values for this component's properties
UTileConnectorComponent::UTileConnectorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;


	Gate=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gate"));
	Gate->SetupAttachment(this);
	Gate->AddRelativeLocation(FVector(0, 0, 10));
	
	auto GateMesh=ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Arena/Assets/SharedTileAssets/SM_Gate.SM_Gate'"));
	if (GateMesh.Succeeded())
	{
		Gate->SetStaticMesh(GateMesh.Object);
	}
	
	Portal =CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Portal"));
	Portal->SetupAttachment(this);
	auto PortalMesh=ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Arena/Assets/DungeonAssets/Portal.Portal'"));
	if (PortalMesh.Succeeded())
	{
		Portal->SetStaticMesh(PortalMesh.Object);
		Portal->AddRelativeLocation({0,0,10});
		Portal->SetVisibility(false);
	}

	PortalEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PortalEffect"));
	PortalEffect->SetupAttachment(Portal);
	auto PortalEffectSystem = ConstructorHelpers::FObjectFinder<UNiagaraSystem>(TEXT("NiagaraSystem'/Game/Arena/VFX/MiscEffects/NS_DoorPortal.NS_DoorPortal'"));
	if (PortalEffectSystem.Succeeded())
	{
		PortalEffect->SetAsset(PortalEffectSystem.Object);
		PortalEffect->SetVisibility(false);
	}

	ConnectorTrigger=CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Trigger"));
	ConnectorTrigger->SetupAttachment(this);

	ConnectorTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ConnectorTrigger->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ConnectorTrigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	FVector BoxExtent(50.f, 200.f, 300.f);
	ConnectorTrigger->InitBoxExtent(BoxExtent);
	ConnectorTrigger->SetHiddenInGame(false);
}



// Called when the game starts
void UTileConnectorComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwnerRole()==ROLE_Authority) //only respond to overlap delegate on server
	{
		ConnectorTrigger->OnComponentBeginOverlap.AddDynamic(this, &UTileConnectorComponent::OnOverlapPawn);
	}

	auto Tile=Cast<ATileBase>(GetOwner());
	if (Tile)
	{
		Tile->OnEnterTile.AddUObject(this, &UTileConnectorComponent::OnEnterTile);
	}
	
}

void UTileConnectorComponent::TransitionToTile()
{
	ATileBase* Owner=Cast<ATileBase>(GetOwner());
	if (Owner)
	{
		AActor* OwningPlayer=Owner->GetActiveActor();
		Owner->LeaveTile();
		EDirection InvertedDirection=EDirection::North;
		switch (ConnectorDirection)
		{
		case EDirection::North:
			InvertedDirection=EDirection::South;
			break;
		case EDirection::East:
			InvertedDirection=EDirection::West;
			break;
		case EDirection::South:
			InvertedDirection=EDirection::North;
			break;
		case EDirection::West:
			InvertedDirection=EDirection::East;
			break;
		}
		ConnectingTile->EnterTile(OwningPlayer, InvertedDirection);
	}
}

void UTileConnectorComponent::FindConnectingTile(bool& bSuccess, ATileBase*& TileHit)
{
	FHitResult HitResult;
	FVector Start=GetComponentLocation()-GetComponentRotation().Vector()*1000.f;
	Start.Z+=5000.f;
	FVector End=Start;
	End.Z-=5000.f;

	FCollisionQueryParams Params;
	bSuccess = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_GameTraceChannel1);
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 10.f, 0, 5.f);
	TileHit = Cast<ATileBase>(HitResult.Actor);
}

void UTileConnectorComponent::OnOverlapPawn(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto OwningTile=Cast<ATileBase>(GetOwner());
	checkf(OwningTile, TEXT("Tile Connector is not owned by a valid Tile"))
	bool bEnemiesLeft=OwningTile->GetEnemiesLeft()>0;

	//Dont do anything if enemies are alive
	if (bEnemiesLeft) return;
	
	ACharacter* Character=Cast<ACharacter>(OtherActor);
	if (Character && Character->IsPlayerControlled())
	{
		
		UE_LOG(LogTemp, Warning, TEXT("Overlapping Character"));
		if (ConnectingTile.Get())
		{
			UE_LOG(LogTemp, Display, TEXT("Tile already present, not spawning new"))
			TransitionToTile();
			return;
		}
		bool bSuccess;
		ATileBase* TileHit;
		FindConnectingTile(bSuccess, TileHit);
		if (bSuccess && TileHit)
		{
			UE_LOG(LogTemp, Display, TEXT("Tile found by raycast, not spawning new"))
			ConnectingTile=TileHit;
			TransitionToTile();
			return;
		}
		
		UTilemapGenerator* Generator=GetWorld()->GetSubsystem<UTilemapGenerator>();
		checkf(Generator, TEXT("Tilemap Generator Subsystem not found"))
		ETileType NewTileType;
		
		uint8 NormalDist=Generator->GetNormalTileDistance();
		uint8 HardDist=Generator->GetHardTileDistance();
		if (FMath::Abs(OwningTile->GetIndex().Key)<NormalDist && FMath::Abs(OwningTile->GetIndex().Value)<NormalDist)
		{
			NewTileType=ETileType::Easy;
		}
		else if (FMath::Abs(OwningTile->GetIndex().Key)<HardDist && FMath::Abs(OwningTile->GetIndex().Value)<HardDist)
		{
			NewTileType=ETileType::Normal;
		}
		else
		{
			NewTileType=ETileType::Hard;
		}

		ConnectingTileClass->WaitUntilComplete();

		
		
		
		ConnectingTile=Generator->CreateNewTileFromLoadedObject(Cast<UClass>(ConnectingTileClass->GetLoadedAsset()), OwningTile, NewTileType, ConnectorDirection, GetOwner()->GetOwner());
		TPair<int8, int8> Index=OwningTile->GetNeighbourIndex(ConnectorDirection);
		UE_LOG(LogTemp, Display, TEXT("Spawning Neighbouring Tile at Index %i, %i"), Index.Key, Index.Value)
		ConnectingTile->SetIndex(Index.Key, Index.Value);
		TransitionToTile();
	}
	
}

void UTileConnectorComponent::OnEnterTile(AActor* EnteringActor)
{
	//check for connecting tile, if none found preload class async
	if (!ConnectingTile)
	{
		bool TileFound;
		ATileBase* TileHit;
		FindConnectingTile(TileFound, TileHit);
		if (!TileFound)
		{
			UTilemapGenerator* Generator=GetWorld()->GetSubsystem<UTilemapGenerator>();
			ConnectingTileClass=Generator->AsyncRequestTileClass(TileClassLoaded);
		}
		else
		{
			ConnectingTile=TileHit;
		}
		
	}
	
}

void UTileConnectorComponent::SetDirection(EDirection InDirection)
{
	ConnectorDirection=InDirection;
	FVector ActorCenter =GetOwner()->GetActorLocation();
	FVector ComponentCenter=GetComponentLocation();

	FRotator Rot=UKismetMathLibrary::FindLookAtRotation(ComponentCenter, ActorCenter);
	SetWorldRotation(Rot);
	
}

void UTileConnectorComponent::UnlockPortal_Implementation()
{
	Portal->SetVisibility(true);
	PortalEffect->SetVisibility(true);
}

