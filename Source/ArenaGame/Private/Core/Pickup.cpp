// Copyright Runescripters 2021


#include "Core/Pickup.h"

#include "ArenaGame/ArenaGame.h"
#include "GAS/GASAbilitySpell.h"

// Sets default values
APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = false;

}

TSubclassOf<UObject> APickup::PickupItemClass()
{
	OnPickupItem.Broadcast();
	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, this, &APickup::ForwardDestroy, DestructionDelay, false);

	//disable collision to prevent multiple pickups
	SetActorEnableCollision(false);
	return ItemClass;
}

EItemType APickup::GetItemType() const
{
	if(ItemClass->IsBasedOnArchetype(UGASAbilitySpell::StaticClass()))
	{
		return EItemType::Spell;
	}
	else
	{
		return EItemType::Rune;
	}
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	
}

void APickup::ForwardDestroy()
{
	UE_LOG(LogSilvaGame, Display, TEXT("Destroying Pickup Actor"))
	Destroy();
}


