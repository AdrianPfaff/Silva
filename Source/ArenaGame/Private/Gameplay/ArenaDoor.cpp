// Copyright Runescripters 2021


#include "Gameplay/ArenaDoor.h"

#include "Core/DuelGameMode.h"

// Sets default values
AArenaDoor::AArenaDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void AArenaDoor::OpenDoorAfterDelay(float Delay)
{
	GetWorldTimerManager().SetTimer(DoorOpenHandle, this, &AArenaDoor::OpenDoor, Delay);
}

void AArenaDoor::OpenDoor_Implementation()
{
	ReceiveOpenDoor();
}

void AArenaDoor::CloseDoor_Implementation()
{
	ReceiveCloseDoor();
}

// Called when the game starts or when spawned
void AArenaDoor::BeginPlay()
{
	Super::BeginPlay();
	
	if (DoorIndex>=0) //Register door for Duel Arena if possible
	{
		if (auto GameMode=GetWorld()->GetAuthGameMode<ADuelGameMode>())
		{
			GameMode->RegisterDoor(this, DoorIndex);
		}
	}
}

