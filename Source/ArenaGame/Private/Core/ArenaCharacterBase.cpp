// Copyright Runescripters 2021


#include "Core/ArenaCharacterBase.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/GASAbilityBase.h"
#include "GAS/GASAttributeSet.h"
#include "GAS/GASComponent.h"


// Sets default values
AArenaCharacterBase::AArenaCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent=CreateDefaultSubobject<UGASComponent>(TEXT("Ability System Component"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	Attributes=CreateDefaultSubobject<UGASAttributeSet>(TEXT("Attributes"));
	
}

// Called when the game starts or when spawned
void AArenaCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	
}


// Called every frame
void AArenaCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

// Called to bind functionality to input
void AArenaCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
}

UAbilitySystemComponent* AArenaCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AArenaCharacterBase::InitializeAttributes()
{
	if (AbilitySystemComponent && DefaultAttributeSetterEffect)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeSetterEffect, 1, EffectContext);

		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle GEHandle=AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
		
	}
}

void AArenaCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	//Client GAS init
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	InitializeAttributes();

	if (AbilitySystemComponent && InputComponent)
	{
		const FGameplayAbilityInputBinds Binds("Confirm",
			"Cancel",
			"EGASAbilityInputID",
			static_cast<int32>(EGASAbilityInputID::Confirm),
			static_cast<int32>(EGASAbilityInputID::Cancel));
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, Binds);
	}
}

void AArenaCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//Server GAS init
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	InitializeAttributes();
	SetOwner(NewController);
}

