// Copyright Runescripters 2021


#include "GAS/GASAbilitySpell.h"

#include "AbilitySystemComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"
#include "GAS/Spell.h"
#include "UI/DA_InventoryItemInfo.h"

void UGASAbilitySpell::InjectRunes(TArray<URuneBase*> InRunes)
{
	Runes=InRunes;
}

void UGASAbilitySpell::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	
	Super::OnAvatarSet(ActorInfo, Spec);

	if (ActivateAbilityOnGranted)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, true);
	}
}

ASpell* UGASAbilitySpell::SpawnSpellActor(TSubclassOf<ASpell> SpellClass, FTransform Transform, AActor* Owner)
{
	ASpell* Spell=Cast<ASpell>(UGameplayStatics::BeginDeferredActorSpawnFromClass(
		this, SpellClass, Transform, ESpawnActorCollisionHandlingMethod::Undefined, Owner));

	Spell->SetOriginAbility(this);
	UE_LOG(LogSilvaGAS, Log, TEXT("Spawning Spell Actor, running Rune Pre Actor Spawn Events"))
	uint8 RuneIndex=0;
	for (const auto& Rune : Runes)
	{
		if (Rune) Rune->PreActorSpawn(Spell, SpellType, this, RuneIndex);
		++RuneIndex;
	}
	UGameplayStatics::FinishSpawningActor(Spell, Transform);
	RuneIndex=0;
	for (const auto& Rune : Runes)
	{
		if (Rune) Rune->PostActorSpawn(Spell, SpellType, this, RuneIndex);
		++RuneIndex;
		
	}
	Spell->EnemyHitBySpell.AddDynamic(this, &UGASAbilitySpell::OnEnemyHitBySpell);
	Spell->OnDestroy.AddDynamic(this, &UGASAbilitySpell::OnSpellActorDestroyed);
	UE_LOG(LogSilvaGAS, Log, TEXT("Spawned Spell Actor, ran Rune Post Actor Spawn Events"))
	return Spell;
}


void UGASAbilitySpell::OnSpellActorDestroyed(FVector Location)
{
	uint8 RuneIndex=0;
	for (const auto& Rune : Runes)
	{
		if (Rune) Rune->OnDestroy(SpellType, this, RuneIndex, Location);
		++RuneIndex;
	}
}

void UGASAbilitySpell::OnExtraSpellActorDestroyed(FVector Location)
{
	uint8 RuneIndex = 0;
	for (const auto& Rune : Runes)
	{
	if (this->SpellType == ESpellType::Passive) // Apply Rune Modifications on SubSpell, if SpellType doesn't match the recursive type
	{
		if (Rune && (Rune->RecursiveType == ERecursiveType::OffensiveRecursive || Rune->RecursiveType == ERecursiveType::NonRecursive))
		{
			Rune->OnDestroy(SpellType, this, RuneIndex, Location);
		}
	}
		else
    	{
    		if (Rune && (Rune->RecursiveType == ERecursiveType::PassiveRecursive || Rune->RecursiveType == ERecursiveType::NonRecursive))
    		{
    			Rune->OnDestroy(SpellType, this, RuneIndex, Location);
    		}
    	}
    	++RuneIndex;	
	}
	UE_LOG(LogSilvaGAS, Log, TEXT("Apply Extra Runes on Destroy"));
}

FPrimaryAssetId UGASAbilitySpell::GetPrimaryAssetId() const
{
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		UClass* BestPrimaryAssetTypeClass = nullptr;
		UClass* SearchPrimaryAssetTypeClass = GetClass()->GetSuperClass();

		// If this is a native class or immediate child of PrimaryDataAsset, return invalid as we are a type ourselves
		if (GetClass()->HasAnyClassFlags(CLASS_Native | CLASS_Intrinsic) || SearchPrimaryAssetTypeClass == UPrimaryDataAsset::StaticClass())
		{
			return FPrimaryAssetId();
		}

		// Starting with parent, look up the hierarchy for a class that is either native, or a blueprint class immediately below PrimaryDataAsset
		while (SearchPrimaryAssetTypeClass)
		{
			if (SearchPrimaryAssetTypeClass->GetSuperClass() == UPrimaryDataAsset::StaticClass())
			{
				// If our parent is this base class, return this as the best class
				BestPrimaryAssetTypeClass = SearchPrimaryAssetTypeClass;
				break;
			}
			else if (SearchPrimaryAssetTypeClass->HasAnyClassFlags(CLASS_Native | CLASS_Intrinsic))
			{
				// Our parent is the first native class found, use that
				BestPrimaryAssetTypeClass = SearchPrimaryAssetTypeClass;
				break;
			}
			else
			{
				SearchPrimaryAssetTypeClass = SearchPrimaryAssetTypeClass->GetSuperClass();
			}
		}

		if (BestPrimaryAssetTypeClass)
		{
			// If this is a native class use the raw name if it's a blueprint use the package name as it will be missing _C
			FName PrimaryAssetType = BestPrimaryAssetTypeClass->HasAnyClassFlags(CLASS_Native | CLASS_Intrinsic) ? BestPrimaryAssetTypeClass->GetFName() : FPackageName::GetShortFName(BestPrimaryAssetTypeClass->GetOutermost()->GetFName());

			return FPrimaryAssetId(PrimaryAssetType, FPackageName::GetShortFName(GetOutermost()->GetName()));
		}

		// No valid parent class found, return invalid
		return FPrimaryAssetId();
	}


	// Data assets use Class and ShortName by default, there's no inheritance so class works fine
	return FPrimaryAssetId(GetClass()->GetFName(), GetFName());
}

ASpell* UGASAbilitySpell::SpawnSpellWithoutModifiers(TSubclassOf<ASpell> SpellClass, FTransform Transform, AActor* Owner, FLinearColor Color)
{
	ASpell* Spell=Cast<ASpell>(UGameplayStatics::BeginDeferredActorSpawnFromClass(
		this, SpellClass, Transform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, Owner));
	Spell->SpellTint = Color;
	Spell->SetOriginAbility(this);
	uint8 RuneIndex = 0;
	for (const auto& Rune : Runes)
	{
		
		if (this->SpellType == ESpellType::Passive) // Apply Rune Modifications on SubSpell, if SpellType doesn't match the recursive type
		{
			if (Rune && (Rune->RecursiveType == ERecursiveType::OffensiveRecursive || Rune->RecursiveType == ERecursiveType::NonRecursive))
			{
				Rune->PreActorSpawn(Spell, SpellType, this, RuneIndex);
			}
		}
		else
		{
			if (Rune && (Rune->RecursiveType == ERecursiveType::PassiveRecursive || Rune->RecursiveType == ERecursiveType::NonRecursive))
			{
				Rune->PreActorSpawn(Spell, SpellType, this, RuneIndex);
			}
		}
		++RuneIndex;
	}
	UGameplayStatics::FinishSpawningActor(Spell, Transform);
	
	for (const auto& Rune : Runes)
	{
		if (this->SpellType == ESpellType::Passive) // Apply Rune Modifications on SubSpell, if SpellType doesn't match the recursive type
			{
			if (Rune && (Rune->RecursiveType == ERecursiveType::OffensiveRecursive || Rune->RecursiveType == ERecursiveType::NonRecursive))
			{
				Rune->PostActorSpawn(Spell, SpellType, this, RuneIndex);
			}
			}
		else
		{
			if (Rune && (Rune->RecursiveType == ERecursiveType::PassiveRecursive || Rune->RecursiveType == ERecursiveType::NonRecursive))
			{
				Rune->PostActorSpawn(Spell, SpellType, this, RuneIndex);
			}
		}
		++RuneIndex;
		
	}
	if (Spell->bCanSpawnSubSpellActor)Spell->EnemyHitBySpell.AddDynamic(this, &UGASAbilitySpell::OnEnemyHitBySpellNoModifiers);
	if (Spell->bCanSpawnSubSpellActor)Spell->OnDestroy.AddDynamic(this, &UGASAbilitySpell::OnExtraSpellActorDestroyed);
	return Spell;
}

void UGASAbilitySpell::TriggerPassiveEffect()
{
	ApplyPassiveEffectAndRunes();
}

void UGASAbilitySpell::AppendGameplayEffectTags(const FName& InTag)
{
	TagsToAppend.Add(FGameplayTag::RequestGameplayTag(InTag));
}

void UGASAbilitySpell::RemoveGameplayEffectTag(const FName& InTag)
{
	TagsToAppend.Remove(FGameplayTag::RequestGameplayTag(InTag));
}

UTexture2D* UGASAbilitySpell::GetIcon() const
{
	if (!ensure(ItemInfo)) return nullptr;

	return ItemInfo->Icon;
}

const FText& UGASAbilitySpell::GetDisplayName() const
{
	if (!ensure(ItemInfo)) return FText::GetEmpty();
	
	return ItemInfo->DisplayName;
}

const FText& UGASAbilitySpell::GetDescription() const
{
	if (!ensure(ItemInfo)) return FText::GetEmpty();

	return ItemInfo->Description;
}

int32 UGASAbilitySpell::GetGold() const
{
	if (!ensure(ItemInfo)) return -1;

	return ItemInfo->ItemPrice;
}

void UGASAbilitySpell::PrepareGameplayEffectAndApply(AArenaCharacterBase* EnemyHit, bool bNoModifiers)
{
	if (!ensureMsgf(BaseEffectClass, TEXT("Effect Class invalid, cant instantiate"))) return;
	
	UGameplayEffect* GameplayEffect=NewObject<UGameplayEffect>(this, BaseEffectClass);
	if (!bNoModifiers)
	{
		AddRuneCalculationsToEffect(*GameplayEffect);
	}
	auto AbilityOwner=Cast<IAbilitySystemInterface>(GetActorInfo().OwnerActor.Get());
	checkf(AbilityOwner, TEXT("Ability Owner does not implement Ability System Interface"))
	for (const auto& Tag: TagsToAppend)
	{
		GameplayEffect->InheritableGameplayEffectTags.AddTag(Tag);
	}
	
	AbilityOwner->GetAbilitySystemComponent()->ApplyGameplayEffectToTarget(GameplayEffect, EnemyHit->GetAbilitySystemComponent(), 0, AbilityOwner->GetAbilitySystemComponent()->MakeEffectContext());
}

void UGASAbilitySpell::OnEnemyHitBySpell(AArenaCharacterBase* EnemyHit)
{
	UE_LOG(LogSilvaGAS, Log, TEXT("Enemy Hit by Spell"))
	uint8 RuneIndex=0;
	for (const auto& Rune : Runes)
	{
		if (Rune) Rune->OnEnemyHit(EnemyHit, SpellType, this, RuneIndex);
		++RuneIndex;
	}
	PrepareGameplayEffectAndApply(EnemyHit, false);
}

void UGASAbilitySpell::OnEnemyHitBySpellNoModifiers(AArenaCharacterBase* EnemyHit)
{
	UE_LOG(LogSilvaGAS, Display, TEXT("Enemy Hit by Spell that uses no modifier runes"))
	uint8 RuneIndex = 0;
	for (const auto& Rune : Runes)
	{
		if (this->SpellType == ESpellType::Passive) // Apply Rune Modifications on SubSpell, if SpellType doesn't match the recursive type
			{
			if (Rune && (Rune->RecursiveType == ERecursiveType::OffensiveRecursive || Rune->RecursiveType == ERecursiveType::NonRecursive))
			{
				Rune->OnEnemyHit(EnemyHit, SpellType, this, RuneIndex);
			}
			}
		else
		{
			if (Rune && (Rune->RecursiveType == ERecursiveType::PassiveRecursive || Rune->RecursiveType == ERecursiveType::NonRecursive))
			{
				Rune->OnEnemyHit(EnemyHit, SpellType, this, RuneIndex);
			}
		}
		++RuneIndex;
		
	}

	PrepareGameplayEffectAndApply(EnemyHit, true);
}

void UGASAbilitySpell::AddRuneCalculationsToEffect(UGameplayEffect& OutEffect)
{
	for (const auto& Rune : Runes)
	{
		if (!Rune) continue;;
		
		if (SpellType==ESpellType::Offensive)
		{
			OutEffect.Modifiers.Append(Rune->GetOffensiveModifiers());
		}
		else
		{
			OutEffect.Modifiers.Append(Rune->GetPassiveModifiers());
		}
	}
	
	
}

void UGASAbilitySpell::ApplyPassiveEffectAndRunes()
{
	UE_LOG(LogSilvaGAS, Log, TEXT("Applying passive effect to self"))
	uint8 RuneIndex=0;
	for (const auto& Rune : Runes)
	{
		if (Rune) Rune->OnPassiveEffectTrigger(this);
		++RuneIndex;
	}
	if (!ensureMsgf(BaseEffectClass, TEXT("Effect Class invalid, cant instantiate"))) return;
	UGameplayEffect* GameplayEffect=NewObject<UGameplayEffect>(this, BaseEffectClass);
	AddRuneCalculationsToEffect(*GameplayEffect);
	auto AbilityOwner=Cast<IAbilitySystemInterface>(GetActorInfo().OwnerActor.Get());
	checkf(AbilityOwner, TEXT("Ability Owner does not implement Ability System Interface"))

	auto Context=AbilityOwner->GetAbilitySystemComponent()->MakeEffectContext();
	AbilityOwner->GetAbilitySystemComponent()->ApplyGameplayEffectToSelf(GameplayEffect, 1.f, Context);

	
}
