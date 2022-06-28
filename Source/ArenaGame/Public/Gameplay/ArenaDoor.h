// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArenaDoor.generated.h"

UCLASS()
class ARENAGAME_API AArenaDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AArenaDoor();
	
	void OpenDoorAfterDelay(float Delay);
	

	UFUNCTION(NetMulticast, Reliable)
	void OpenDoor();

	UFUNCTION(NetMulticast, Reliable)
	void CloseDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="Open Door"))
	void ReceiveOpenDoor();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="Close Door"))
	void ReceiveCloseDoor();

	UPROPERTY(EditInstanceOnly)
	int8 DoorIndex=-1;

	FTimerHandle DoorOpenHandle;
};
