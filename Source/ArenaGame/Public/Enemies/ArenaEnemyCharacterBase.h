// Copyright Runescripters 2021

#pragma once


#include "CoreMinimal.h"
#include "DeathResponseInterface.h"
#include "Core/ArenaCharacterBase.h"

#include "ArenaEnemyCharacterBase.generated.h"

struct FOnAttributeChangeData;
class UBehaviorTree;

/**
 * 
 */
UCLASS(Abstract)
class ARENAGAME_API AArenaEnemyCharacterBase : public AArenaCharacterBase, public IDeathResponseInterface
{
	GENERATED_BODY()

public:
	AArenaEnemyCharacterBase();
	
	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION(BlueprintCallable)
	void SetMovementSpeed(float NewSpeed, float& OutOldSpeed);

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnDeath"))
	void ReceiveDeath();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UBehaviorTree* BehaviorTree;

private:
	
	void OnHealthChanged(const FOnAttributeChangeData& Data);
	
	void Death();

	
	
	
};
