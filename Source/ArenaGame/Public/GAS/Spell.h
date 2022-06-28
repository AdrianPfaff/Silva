// Copyright Runescripters 2021

#pragma once


#include "CoreMinimal.h"
#include "Core/ArenaPlayerCharacter.h"
#include "Enemies/ArenaEnemyCharacterBase.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Spell.generated.h"

class UGASAbilitySpell;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnemyHitBySpell, AArenaCharacterBase*, EnemyHit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDestroy, FVector, Location);

UENUM(BlueprintType)
enum class ESpellActorType : uint8
{
	Projectile UMETA(DisplayName="Projectile"),
	NonProjectile UMETA(DisplayName="Non-Projectile")
};

UENUM(BlueprintType)
enum class EBranch : uint8
{
	ReboundOff,
	ReboundOn
};

//Physical Representation of the Spell in the World
UCLASS()
class ARENAGAME_API ASpell : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpell();

	virtual void K2_DestroyActor() override;
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf=Target, HideSelfPin=true))
	void SetIgnoreOwnerOnMove(UPrimitiveComponent* TargetComponent);

	UPROPERTY(BlueprintCallable)
	FEnemyHitBySpell EnemyHitBySpell;
	UPROPERTY()
	FOnDestroy OnDestroy;

	
	UPROPERTY(BlueprintReadWrite)
	FLinearColor SpellTint {0.5, 0.5, 0.5 ,1};

	UFUNCTION(BlueprintCallable)
	const ESpellActorType GetSpellActorType () {return SpellActorType;}

	void SetOriginAbility(UGASAbilitySpell* InAbility) {OriginAbility=InAbility; }

	UFUNCTION(BlueprintCallable)
	void ApplySilenceToTarget(AArenaPlayerCharacter* Target, TSubclassOf<UGameplayEffect> InEffect);

	UFUNCTION(BlueprintCallable, Category = "Stuff", Meta = (ExpandEnumAsExecs = "Branches"))
	void HandleBounceRune(UProjectileMovementComponent* ProjectileTarget, EBranch& Branches);

	UFUNCTION(BlueprintCallable)
	AActor* SpawnExtraActor(TSubclassOf<AActor> ActorToSpawn, FTransform Transform, AActor* InOwner);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCanSpawnSubSpellActor {true};

protected:
	UPROPERTY(BlueprintReadOnly)
	UGASAbilitySpell* OriginAbility;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ESpellActorType SpellActorType = ESpellActorType::NonProjectile;
	
	UPROPERTY(BlueprintReadWrite)
	int ReboundEnabled {0};

	
	
};
