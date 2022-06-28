// Copyright Runescripters 2021


#include "Gameplay/Gold.h"

// Sets default values
AGold::AGold()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AGold::BeginPlay()
{
	Super::BeginPlay();

	int32 InitialAmount=GoldAmount;
	GoldAmount-=GoldAmount*ItemsSpawned*ItemReductionCoefficient;
	GoldAmount-=GoldAmount*PotionsSpawned*PotionReductionCoefficient;
	
	GoldAmount=FMath::Clamp(GoldAmount, 0, InitialAmount);
}
