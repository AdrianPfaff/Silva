// Copyright Runescripters 2021


#include "Gameplay/Chest.h"

#include "Core/DungeonGameMode.h"
#include "Engine/AssetManager.h"
#include "GAS/GASAbilitySpell.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AChest::AChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void AChest::FillItems()
{
	if(auto GameMode=Cast<ADungeonGameMode>(GetWorld()->GetAuthGameMode()))
	{
		bool bShouldSpawnRune1=UKismetMathLibrary::RandomBoolWithWeight(0.35f);
		bool bShouldSpawnRune2=UKismetMathLibrary::RandomBoolWithWeight(0.35f);
		bool bShouldSpawnSpell=UKismetMathLibrary::RandomBoolWithWeight(0.15f);
		bool bShouldSpawnPotion=UKismetMathLibrary::RandomBoolWithWeight(0.10f);
		
		UClass* SpellClass=GameMode->GetRandomSpellClass();
		UClass* RuneClass1=GameMode->GetRandomRuneClass();
		UClass* RuneClass2=GameMode->GetRandomRuneClass();
		UClass* PotionClass=GameMode->GetPotionClass();
		if (RuneClass1 && bShouldSpawnRune1)
		{
			ContainedItems.Add(RuneClass1);
		}
		if (RuneClass2 && bShouldSpawnRune2)
		{
			ContainedItems.Add(RuneClass2);
		}
		
		if (SpellClass && bShouldSpawnSpell)
		{
			ContainedItems.Add(SpellClass);
		}

		if (PotionClass&& bShouldSpawnPotion)
		{
			Potions.Add(PotionClass);
		}
		
	}
}

// Called when the game starts or when spawned
void AChest::BeginPlay()
{
	Super::BeginPlay();
	
	
}

