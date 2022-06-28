// Copyright Runescripters 2021


#include "Core/PlayerInventoryComponent.h"

#include "Core/ArenaPlayerCharacter.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

UPlayerInventoryComponent::UPlayerInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	SpellSlots.Add(FPlayerSpellSlot());
	SpellSlots.Add(FPlayerSpellSlot());
	SpellSlots.Add(FPlayerSpellSlot());

	for (int i = 0; i < MaxItems; ++i)
	{
		Items.Add(nullptr);
	}
}

void UPlayerInventoryComponent::SetSpellInSlot_Implementation(EGASAbilityInputID InputID,
	FGameplayAbilitySpecHandle Handle, UGASAbilitySpell* Spell)
{
	int32 SlotIndex=static_cast<int32>(InputID)-3;
	if (SlotIndex>2 || SlotIndex<0)
	{
		return;
	}
	SpellSlots[SlotIndex].AbilityHandle=Handle;
	SpellSlots[SlotIndex].AbilitySpell=Spell;
}

void UPlayerInventoryComponent::SetRuneInSpell_Implementation(EGASAbilityInputID InInputID, URuneBase* NewRune,
	uint8 RuneSlot)
{
	if (RuneSlot<0 || RuneSlot>2) return;
	
	int32 SpellSlotIndex=static_cast<int32>(InInputID)-3;
	
	if (SpellSlotIndex>2 || SpellSlotIndex<0) return;
	
	SpellSlots[SpellSlotIndex].Runes[RuneSlot]=NewRune;
}

bool UPlayerInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch,
                                                    FReplicationFlags* RepFlags)
{
	for (const auto Item : Items)
	{
		Channel->ReplicateSubobject(Item, *Bunch, *RepFlags);
	}

	return true;
}

void UPlayerInventoryComponent::ReplaceItemAtIndex_Implementation(UObject* InNewItem, int32 Index, UObject* OutOldItem)
{
	if (!Items.IsValidIndex(Index)) return;

	OutOldItem=Items[Index];
	Items[Index]=InNewItem;
}

void UPlayerInventoryComponent::ReplaceInventory(TArray<FPlayerSpellSlot>& InSlots, TArray<UObject*>& InItems)
{
	SpellSlots=InSlots;
	Items=InItems;
	auto PlayerChar=Cast<AArenaPlayerCharacter>(GetOwner());
	ensure(PlayerChar);
	uint8 InputID=3;
	for (auto& Slot : SpellSlots)
	{
		PlayerChar->GrantNewAbilityInSlot(Slot.AbilitySpell ? Slot.AbilitySpell->GetClass() : nullptr, static_cast<EGASAbilityInputID>(InputID));
		++InputID;
	}
}

void UPlayerInventoryComponent::CopyInventory(TArray<FPlayerSpellSlot>& OutSlots, TArray<UObject*>& OutItems)
{
	OutSlots=SpellSlots;
	OutItems=Items;
}

bool UPlayerInventoryComponent::PurchaseItem(TSubclassOf<UObject> ItemToBuy, int32 Price)
{
	if (Price > Gold || !ItemToBuy || !HasFreeSlots())
	{
		 
		return false;
	}
	else
	{
		PurchaseItemServer(ItemToBuy, Price);
		return true;
	}
}

bool UPlayerInventoryComponent::PurchasePotion(int32 Price)
{
	if (Price < 0 || Price>Gold)
	{
		return false;
	}
	PurchasePotionServer(Price);
	return true;
}

void UPlayerInventoryComponent::PurchasePotionServer_Implementation(int32 Price)
{
	Gold-=Price;
	OnGoldChanged.Broadcast(Gold);
}

void UPlayerInventoryComponent::PurchaseItemServer_Implementation(TSubclassOf<UObject> ItemToBuy, int32 Price)
{
	UpdateGoldValue(-Price);
	auto Item=NewObject<UObject>(GetTransientPackage(), ItemToBuy);
	if (const auto Rune = Cast<URuneBase>(Item))
	{
		Rune->SetOwner(GetOwner());
	}
	AddItem(Item);
	
}

bool UPlayerInventoryComponent::HasFreeSlots() const
{
	return Items.Contains(nullptr);
}

const TArray<FPlayerSpellSlot>& UPlayerInventoryComponent::GetSpellSlots() const
{
	return SpellSlots;
}

void UPlayerInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPlayerInventoryComponent, SpellSlots)
	DOREPLIFETIME(UPlayerInventoryComponent, Items)
	DOREPLIFETIME(UPlayerInventoryComponent, Gold)
}

void UPlayerInventoryComponent::AddItem(UObject* Item)
{
	if (!Item)
	{
		return;
	}
	auto Index=Items.Find(nullptr);
	if (Index!=INDEX_NONE)
	{
		Items[Index]=Item;
	}
}

void UPlayerInventoryComponent::SwapIndices_Implementation(int32 A, int32 B)
{

	Items.Swap(A, B);
}

void UPlayerInventoryComponent::RemoveItem_Implementation(UObject* Item)
{
	int32 Index=Items.Find(Item);
	if (Index!=INDEX_NONE)
	{
		Items[Index]=nullptr;
	}
}

const TArray<UObject*>& UPlayerInventoryComponent::GetItems() const
{
	return Items;
}

void UPlayerInventoryComponent::PickupGold(int32 Amount)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	UpdateGoldValue(Amount);
}

void UPlayerInventoryComponent::OnRep_GoldChanged()
{
	OnGoldChanged.Broadcast(Gold);
}

// Called when the game starts
void UPlayerInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UPlayerInventoryComponent::UpdateGoldValue(float Difference)
{
	Gold+=Difference;
	OnGoldChanged.Broadcast(Gold);
}
