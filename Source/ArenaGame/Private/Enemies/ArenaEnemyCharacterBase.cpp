// Copyright Runescripters 2021


#include "Enemies/ArenaEnemyCharacterBase.h"

#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/GASAttributeSet.h"
#include "GAS/GASComponent.h"


AArenaEnemyCharacterBase::AArenaEnemyCharacterBase()
{
	
}

void AArenaEnemyCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	auto AIController=Cast<AAIController>(NewController);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UGASAttributeSet::GetHealthAttribute())
	.AddLambda([this](const FOnAttributeChangeData Data){ this->OnHealthChanged(Data);});

	checkf(AIController, TEXT("AI Character should only be possessed by an AI Controller"))
	AIController->RunBehaviorTree(BehaviorTree);

	
}

void AArenaEnemyCharacterBase::SetMovementSpeed(float NewSpeed, float& OutOldSpeed)
{
	OutOldSpeed=GetCharacterMovement()->MaxWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeed=NewSpeed;
}

void AArenaEnemyCharacterBase::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (FMath::IsNearlyZero(Data.NewValue) || Data.NewValue<=0.f)
	{
		Death();
	}
	
}


void AArenaEnemyCharacterBase::Death()
{
	OnDeath.Broadcast(this);
	ReceiveDeath();
}
	
