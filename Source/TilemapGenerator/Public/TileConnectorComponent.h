// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "EnumLibrary.h"
#include "Components/BoxComponent.h"
#include "Engine/StreamableManager.h"
#include "NiagaraComponent.h"
#include "TileConnectorComponent.generated.h"


struct FStreamableHandle;
class ATileBase;

/**
 * Component to aid interaction between tiles, providing functionality by triggering transition to neighboring tiles
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TILEMAPGEN_API UTileConnectorComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTileConnectorComponent();
	

	UFUNCTION(BlueprintCallable, BlueprintPure)
	EDirection GetConnectorDirection() const { return ConnectorDirection; }
	//Selects which direction the connector is pointing to, updating the rotation of the text as well
	UFUNCTION(BlueprintCallable)
	void SetDirection(EDirection InDirection);

	UFUNCTION(Client, Unreliable)
	void UnlockPortal();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void TransitionToTile();
	void FindConnectingTile(bool& bSuccess, ATileBase*& TileHit);


	UPROPERTY(BlueprintReadOnly)
	TSoftObjectPtr<ATileBase> ConnectingTile;

	UPROPERTY(BlueprintReadOnly)
	EDirection ConnectorDirection;
	
	UPROPERTY()
	UStaticMeshComponent* Gate;

	UPROPERTY()
	UStaticMeshComponent* Portal;

	UPROPERTY()
	UNiagaraComponent* PortalEffect;
	
	UPROPERTY()
	UBoxComponent* ConnectorTrigger;

	//bind to overlap collision delegate on server to trigger transitioning to neighbor tile
	UFUNCTION()
	void OnOverlapPawn(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEnterTile(AActor* EnteringActor);

	TSharedPtr<FStreamableHandle> ConnectingTileClass;

	FStreamableDelegate TileClassLoaded;
};
