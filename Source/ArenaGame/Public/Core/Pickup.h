// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UI/InventoryItemWrapper.h"
#include "Pickup.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPickupItemEvent);

UCLASS()
class ARENAGAME_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickup();

	void SetItem(TSubclassOf<UObject> Class) { ItemClass=Class; }
	
	TSubclassOf<UObject> PickupItemClass();

	UFUNCTION(BlueprintPure)
	EItemType GetItemType() const;

	UPROPERTY(BlueprintCallable)
	FPickupItemEvent OnPickupItem;

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowedClasses="RuneBase, GASAbilitySpell", ExposeOnSpawn))
	TSubclassOf<UObject> ItemClass;

private:

	//Forwards destroy to base class to be able to use timer on destroy
	UFUNCTION()
	void ForwardDestroy();
	
	UPROPERTY(EditAnywhere)
	float DestructionDelay=2.f;
	

};
