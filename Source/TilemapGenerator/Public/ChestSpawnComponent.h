// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ChestSpawnComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TILEMAPGEN_API UChestSpawnComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UChestSpawnComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
};
