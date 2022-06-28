// Copyright Runescripters 2021


#include "Core/MainMenuGameMode.h"

#include "Core/ArenaGameInstance.h"

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();
	auto Instance=GetGameInstance<UArenaGameInstance>();
	if (Instance)
	{
		Instance->DestroySession();
	}
}
