// Copyright Runescripters 2021


#include "Gameplay/DuelPlayerStart.h"

#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Core/DuelGameMode.h"

ADuelPlayerStart::ADuelPlayerStart()
{
	Capsule=CreateDefaultSubobject<UCapsuleComponent>(TEXT("Player Capsule"));
	RootComponent=Capsule;
	Capsule->SetCapsuleHalfHeight(96.f);
	Capsule->SetCapsuleRadius(42.f);

#if WITH_EDITORONLY_DATA
	Arrow=CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	if (Arrow)
	{
		Arrow->SetupAttachment(RootComponent);
	}
	

#endif
	
}

void ADuelPlayerStart::BeginPlay()
{
	Super::BeginPlay();

	if (auto DuelMode=Cast<ADuelGameMode>(GetWorld()->GetAuthGameMode()))
	{
		DuelMode->RegisterSpawn(this, StartIndex);
	}
}
