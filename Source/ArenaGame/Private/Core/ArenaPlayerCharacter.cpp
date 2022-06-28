// Copyright Runescripters 2021


#include "Core/ArenaPlayerCharacter.h"

#include "GameModeInterface.h"
#include "NavigationInvokerComponent.h"
#include "TileBase.h"
#include "Components/CapsuleComponent.h"
#include "Core/ArenaGameState.h"
#include "Core/ArenaPlayerController.h"
#include "Core/ArenaPlayerState.h"
#include "Core/DungeonGameMode.h"
#include "Core/Pickup.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/PlayerState.h"
#include "Gameplay/Gold.h"
#include "GAS/GASAbilitySpell.h"
#include "GAS/GASAttributeSet.h"
#include "GAS/GASComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


AArenaPlayerCharacter::AArenaPlayerCharacter()
{
	
	NavigationInvokerComponent=CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("Navigation Invoker"));
	Inventory=CreateDefaultSubobject<UPlayerInventoryComponent>(TEXT("Player Inventory"));
	
}
void AArenaPlayerCharacter::PossessedBy(AController* NewController)
{
	
	Super::PossessedBy(NewController);
}

void AArenaPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AArenaPlayerCharacter, Inventory)
	DOREPLIFETIME(AArenaPlayerCharacter, PlayerStatus)
}

void AArenaPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AArenaPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("PrimarySpell", IE_Pressed,this, &AArenaPlayerCharacter::TriggerPrimarySpell);
	PlayerInputComponent->BindAction("SecondarySpell", IE_Pressed,this, &AArenaPlayerCharacter::TriggerSecondarySpell);
	PlayerInputComponent->BindAction("TertiarySpell", IE_Pressed,this, &AArenaPlayerCharacter::TriggerTertiarySpell);
}

void AArenaPlayerCharacter::GrantNewAbilityInSlot_Implementation(TSubclassOf<UGameplayAbility> InAbility, EGASAbilityInputID InInputID)
{
	int32 Slot=static_cast<int32>(InInputID)-3;
	if (!InAbility && Inventory->GetSpellSlots().IsValidIndex(Slot))
	{
		AbilitySystemComponent->ClearAbility(Inventory->GetSpellSlots()[Slot].AbilityHandle);
		Inventory->SetSpellInSlot(InInputID, FGameplayAbilitySpecHandle(), nullptr);
	}
	else if (Inventory->GetSpellSlots().IsValidIndex(Slot))
	{
		AbilitySystemComponent->ClearAbility(Inventory->GetSpellSlots()[Slot].AbilityHandle);
		FGameplayAbilitySpecHandle NewHandle=AbilitySystemComponent->GiveAbility(InAbility);
		Inventory->SetSpellInSlot(InInputID, NewHandle, Cast<UGASAbilitySpell>(InAbility.GetDefaultObject()));
		
	}
	if (auto PC=GetController<AArenaPlayerController>())
	{
		PC->UpdateSpellIcons();
	}
}

void AArenaPlayerCharacter::TriggerAbilityInSlot(EGASAbilityInputID InInputID)
{
	Server_TriggerAbilityInSlot(InInputID);
}

void AArenaPlayerCharacter::Server_TriggerAbilityInSlot_Implementation(EGASAbilityInputID InInputID)
{
	if (PlayerStatus==EPlayerState::Alive)
	{
		int32 Slot=static_cast<int32>(InInputID)-3;
		AbilitySystemComponent->TryActivateAbility(Inventory->GetSpellSlots()[Slot].AbilityHandle);
	}
}


void AArenaPlayerCharacter::SetSkeletalMesh(USkeletalMesh* NewMesh)
{
	GetMesh()->SetSkeletalMesh(NewMesh);
}

void AArenaPlayerCharacter::OnAbilityActivated(UGameplayAbility* GameplayAbility)
{
	auto Spell=Cast<UGASAbilitySpell>(GameplayAbility);
	auto AbilityHandle=GameplayAbility->GetCurrentAbilitySpecHandle();
	if (!Spell || !AbilityHandle.IsValid())
	{
		UE_LOG(LogSilvaGAS, Error, TEXT("Ability activated, but is not Spell or Handle is invalid"))
		return;
	}
	
	for(const auto& SpellSlot: Inventory->GetSpellSlots())
	{
		if (SpellSlot.AbilityHandle==AbilityHandle)
		{
			Spell->InjectRunes(SpellSlot.Runes);
			UE_LOG(LogSilvaGAS, Display, TEXT("Injected Runes into Spell"))
		}
		
	}
}


void AArenaPlayerCharacter::ResetPowerfluxStacks()
{
	PowerfluxStacks = 0;
	return;
}

void AArenaPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		
		GetWorldTimerManager().SetTimer(InvHandle, this, &AArenaPlayerCharacter::ReplaceInventory, 0.2f, false);
	}
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attributes->GetHealthAttribute()).AddUObject(this, &AArenaPlayerCharacter::HealthChanged);
	
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attributes->GetShieldValueAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{this->ClampShieldVal.Broadcast(Data.NewValue - Data.OldValue);});

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attributes->GetAppliedShieldAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
			{this->ClampAppliedShieldVal.Broadcast(Data.NewValue);});

	AbilitySystemComponent->AbilityCommittedCallbacks.AddLambda([this](UGameplayAbility* CommitedAbility){this->PowerfluxStacks++; PowerfluxStacks = FMath::Clamp<float>(PowerfluxStacks, 0, 10);});
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attributes->GetMovementSpeedAttribute())
	.AddLambda([this](const FOnAttributeChangeData& Data){ this->OnMovementSpeedChanged.Broadcast(Data.NewValue);});

	
	ClampShieldVal.AddDynamic(this, &AArenaPlayerCharacter::UpdateShieldVal);
	ClampAppliedShieldVal.AddDynamic(this, &AArenaPlayerCharacter::UpdateAppliedShieldVal);

	OnMovementSpeedChanged.AddDynamic(this, &AArenaPlayerCharacter::UpdateMovementSpeed);

	if (HasAuthority())
	{
		AbilitySystemComponent->AbilityActivatedCallbacks.AddUObject(this, &AArenaPlayerCharacter::OnAbilityActivated);
		GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AArenaPlayerCharacter::OnPlayerOverlap);
		
	}
}



void AArenaPlayerCharacter::UpdateShieldVal(float NewShield)
{
	AbilitySystemComponent->ApplyModToAttribute(UGASAttributeSet::GetAppliedShieldAttribute(), EGameplayModOp::Additive, NewShield);
}

void AArenaPlayerCharacter::UpdateAppliedShieldVal(float NewShield)
{
	if (NewShield < 0.f)
	{
			AbilitySystemComponent->ApplyModToAttribute(UGASAttributeSet::GetAppliedShieldAttribute(), EGameplayModOp::Override, 0.f);
	}
}

void AArenaPlayerCharacter::Multicast_TriggerPlayerRespawn_Implementation()
{
	ReceivePlayerRespawn();
}

void AArenaPlayerCharacter::TriggerPlayerRespawn()
{
	Multicast_TriggerPlayerRespawn();
}

void AArenaPlayerCharacter::ReplaceInventory()
{
	auto ArenaPlayerState=GetPlayerState<AArenaPlayerState>();
	if (ArenaPlayerState && ArenaPlayerState->ShouldReplaceInventory())
	{
		

		TArray<FPlayerSpellSlot> Spells;
		TArray<UObject*> Items;
		ArenaPlayerState->RetrieveInventory(Spells, Items);
		Inventory->ReplaceInventory(Spells, Items);
		UE_LOG(LogSilvaOnline, Warning, TEXT("Transferring inventory from old player controller, contains %i Spellslots and %i items"), Spells.Num(), Items.Num())
	}
	else
	{
		GetWorldTimerManager().SetTimer(InvHandle, this, &AArenaPlayerCharacter::ReplaceInventory, 0.2f, false);
	}
}

void AArenaPlayerCharacter::TriggerPlayerDeath_Implementation(float RespawnDelay)
{
	if (GetController())
	{
		GetController()->SetIgnoreMoveInput(true);
	}
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bUseControllerDesiredRotation=false;
	}
	
	ReceivePlayerDeath(RespawnDelay);
}

int AArenaPlayerCharacter::GetLockoutStacks() const
{
	int Stacks = 0;

	FGameplayTagContainer CompareCont;
	CompareCont.AddTag(FGameplayTag::RequestGameplayTag(FName("SilenceArmorActive")));
	
	Stacks = AbilitySystemComponent->GetActiveEffectsWithAllTags(CompareCont).Num();
	UE_LOG(LogSilvaGAS, Warning, TEXT("Active Lockout Passives: %i"), Stacks);
	return Stacks;
}

void AArenaPlayerCharacter::OnPlayerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APickup* Pickup=Cast<APickup>(OtherActor);
	if (Pickup && Inventory->HasFreeSlots())
	{
		UClass* ItemClass = Pickup->PickupItemClass();
		auto Item=NewObject<UObject>(GetTransientPackage(), ItemClass);
		if (const auto Rune = Cast<URuneBase>(Item))
		{
			UE_LOG(LogSilvaGAS, Warning, TEXT("Cast To Rune Successful"));
			Rune->SetOwner(this);
		}
		UE_LOG(LogSilvaGame, Display, TEXT("Picking up Item: %s"), *ItemClass->GetName())
		Inventory->AddItem(Item);
		return;
	}
	AGold* Gold=Cast<AGold>(OtherActor);
	if (Gold)
	{
		Inventory->PickupGold(Gold->GoldAmount);
		Gold->Destroy();
	}
}

void AArenaPlayerCharacter::HealthChanged(const FOnAttributeChangeData& Data)
{
	OnHealthChanged.Broadcast(Data.NewValue);
	if (Data.NewValue<=0.f && PlayerStatus!=EPlayerState::Dead)
	{
		float Delay=0;
		PlayerStatus=EPlayerState::Dead;
		UE_LOG(LogSilvaGame, Log, TEXT("Player has died"))
		if (auto GameMode=Cast<IGameModeInterface>(GetWorld()->GetAuthGameMode()))
		{
			
			GameMode->HandlePlayerDeath(this, Delay);
			if (OnTile)
			{
				OnTile->OnPlayerDeath();
			}
		}
		TriggerPlayerDeath(Delay);
	}

	else if (Data.NewValue > AbilitySystemComponent->GetNumericAttribute(Attributes->GetMaxHealthAttribute()))
	{
		AbilitySystemComponent->ApplyModToAttribute(Attributes->GetHealthAttribute(), EGameplayModOp::Override, AbilitySystemComponent->GetNumericAttribute(Attributes->GetMaxHealthAttribute()));
	}

}

void AArenaPlayerCharacter::ToggleFriction()
{
	float Friction = GetCharacterMovement()->FallingLateralFriction;

	FMath::IsNearlyZero(Friction) ? GetCharacterMovement()->FallingLateralFriction = 8.f : GetCharacterMovement()->FallingLateralFriction = 0.f;
	
}

void AArenaPlayerCharacter::NotifyAbilityCommited(FGameplayAbilitySpecHandle Handle, float Cooldown)
{
	int32 SpellIndex=0;
	for (auto Slot : Inventory->GetSpellSlots())
	{
		if (Slot.AbilityHandle==Handle)
		{
			break;
		}
		++SpellIndex;
	}
	EGASAbilityInputID InputID=static_cast<EGASAbilityInputID>(SpellIndex+3);

	OnAbilityCommited.Broadcast(InputID, Cooldown);
	UE_LOG(LogSilvaGAS, Display, TEXT("Ability on Cooldown in Slot: %s, on cooldown for %f seconds"), *UEnum::GetValueAsString(InputID), Cooldown)
	
}

void AArenaPlayerCharacter::UpdateMovementSpeed(float NewValue)
{
	GetCharacterMovement()->MaxWalkSpeed=NewValue;
}