// Copyright Runescripters 2021


#include "UI/RemoveItemBackdrop.h"

#include "Core/ArenaPlayerCharacter.h"
#include "Core/Pickup.h"
#include "Kismet/GameplayStatics.h"
#include "UI/InventoryItemWrapper.h"
#include "UI/NavWidget_InventoryRuneSlot.h"
#include "UI/NavWidget_InventorySpellSlot.h"

void URemoveItemBackdrop::DropItem_Implementation(UInventoryItemWrapper* Item, UNavWidget_InventorySlot* OriginSlot)
{
	auto PlayerPawn=GetOwningPlayerPawn();
	FTransform SpawnTransform;
	if (!Item || !PlayerPawn)
	{
		return;
	}
	SpawnTransform.SetLocation(PlayerPawn->GetActorLocation()+PlayerPawn->GetActorForwardVector()*100.f);
	AActor* Pickup=UGameplayStatics::BeginDeferredActorSpawnFromClass(PlayerPawn, PickupClass, SpawnTransform);
	APickup* CastedPickup=Cast<APickup>(Pickup);
	if (!ensureMsgf(CastedPickup, TEXT("Item drop failed: Invalid Pickup Class")))
	{
		return;
	}
	CastedPickup->SetItem(Item->GetSpell() ? Item->GetSpell()->GetClass() : Item->GetRune()->GetClass());
	auto PlayerCharacter=Cast<AArenaPlayerCharacter>(PlayerPawn);
	UGameplayStatics::FinishSpawningActor(Pickup, SpawnTransform);
	if (auto RuneSlot=Cast<UNavWidget_InventoryRuneSlot>(OriginSlot))
	{
		PlayerCharacter->GetInventoryComponent()->SetRuneInSpell(RuneSlot->GetInputID(), nullptr, RuneSlot->GetRuneIndex());
	} else if (auto SpellSlot=Cast<UNavWidget_InventorySpellSlot>(OriginSlot))
	{
		PlayerCharacter->GrantNewAbilityInSlot(nullptr, SpellSlot->GetInputID());
	}
	else
	{
		UObject* Obj=Item->GetSpell() ? Cast<UObject>(Item->GetSpell()) : Cast<UObject>(Item->GetRune());
		PlayerCharacter->GetInventoryComponent()->RemoveItem(Obj);
	}
	
	
	
}
