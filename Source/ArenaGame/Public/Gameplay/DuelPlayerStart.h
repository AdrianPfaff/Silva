// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "DuelPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class ARENAGAME_API ADuelPlayerStart : public AActor
{
	GENERATED_BODY()

public:
	ADuelPlayerStart();
	
	
	virtual void BeginPlay() override;

protected:

	UPROPERTY(EditInstanceOnly)
	uint8 StartIndex;

	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* Capsule;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	UArrowComponent* Arrow;
#endif
	
	
};
