// Copyright Runescripters 2021


#include "GAS/Spell.h"

#include "GAS/GASComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASpell::ASpell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ASpell::K2_DestroyActor()
{
	OnDestroy.Broadcast(GetActorLocation());
	
	Super::K2_DestroyActor();
}

void ASpell::SetIgnoreOwnerOnMove(UPrimitiveComponent* TargetComponent)
{
	if (TargetComponent)
	{
		TargetComponent->IgnoreActorWhenMoving(GetOwner(), true);
	}
}


void ASpell::ApplySilenceToTarget(AArenaPlayerCharacter* Target, TSubclassOf<UGameplayEffect> InEffect)
{
	auto SourceActor = Cast<AArenaPlayerCharacter>(GetOwner());
	int32 TargetLockoutStacks = Target->GetLockoutStacks();
	if (Target->Tags.Contains("SilenceArmorActive") && SourceActor)
	{
		for (int i = 0; i<TargetLockoutStacks; i++)
		{
			SourceActor->AbilitySystemComponent->BP_ApplyGameplayEffectToSelf(InEffect, 0, SourceActor->AbilitySystemComponent->MakeEffectContext());
		}
	}
}

void ASpell::HandleBounceRune(UProjectileMovementComponent* ProjectileTarget, EBranch& Branches)
{
	if (ProjectileTarget)
	{
	if (ReboundEnabled <= 0)
    	{
    		Branches = EBranch::ReboundOff;
    	}
    	else
    	{
    		ProjectileTarget->bShouldBounce = true;
    		ReboundEnabled -= 1;
    		Branches = EBranch::ReboundOn;
    	}	
	}
}

AActor* ASpell::SpawnExtraActor(TSubclassOf<AActor> ActorToSpawn, FTransform Transform, AActor* InOwner)
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = InOwner;
	return GetWorld()->SpawnActor<AActor>(ActorToSpawn, Transform, SpawnParameters);
}

// Called when the game starts or when spawned
void ASpell::BeginPlay()
{
	Super::BeginPlay();
	
}

