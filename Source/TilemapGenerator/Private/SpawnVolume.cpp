// Copyright Runescripters 2021


#include "SpawnVolume.h"


USpawnVolume::USpawnVolume()
{
	bCanEverAffectNavigation=false;
	UPrimitiveComponent::SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UActorComponent::SetComponentTickEnabled(false);
}
