// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gold.generated.h"

UCLASS()
class ARENAGAME_API AGold : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGold();

	

	UPROPERTY(EditAnywhere)
	int32 GoldAmount;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly,meta=(ExposeOnSpawn=true))
	int32 ItemsSpawned=0;

	UPROPERTY(BlueprintReadOnly,meta=(ExposeOnSpawn=true))
	int32 PotionsSpawned=0;

	UPROPERTY(EditDefaultsOnly)
	float PotionReductionCoefficient=0.1f;

	UPROPERTY(EditDefaultsOnly)
	float ItemReductionCoefficient=0.3f;
};
