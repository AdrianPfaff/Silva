// Copyright Runescripters 2021


#include "GAS/GASAbilityBase.h"

#include "Core/ArenaPlayerCharacter.h"

UGASAbilityBase::UGASAbilityBase()
{
}

bool UGASAbilityBase::CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags)
{
	bool bSuccess=Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
	auto PlayerCharacter=Cast<AArenaPlayerCharacter>(ActorInfo->AvatarActor);
	if (PlayerCharacter)
	{
		PlayerCharacter->NotifyAbilityCommited(Handle, GetCooldownTimeRemaining());
	}
	
	return bSuccess;
}

