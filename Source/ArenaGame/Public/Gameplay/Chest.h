// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chest.generated.h"


UCLASS()
class ARENAGAME_API AChest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChest();

	UFUNCTION(BlueprintCallable)
	void FillItems();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TArray<TSubclassOf<UObject>> ContainedItems;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TArray<TSubclassOf<AActor>> Potions;
	
};
