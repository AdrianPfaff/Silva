// Copyright Runescripters 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "ArenaCharacterBase.generated.h"



/**
 * @brief Base Character class for all player and AI characters. Implements shared base functionality.
 */
UCLASS(Abstract)
class ARENAGAME_API AArenaCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AArenaCharacterBase();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** Initializes attributes to the default values specified in @property DefaultAttributeSetterEffect */
	virtual void InitializeAttributes();

	/** Replicates Player State from the Server to all clients, setting up bindings for GAS. Not used for AI Characters */
	virtual void OnRep_PlayerState() override;

	/** When possessed by a new controller reinitializes attributes to defaults and sets up GAS bindings */
	virtual void PossessedBy(AController* NewController) override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	

	/** Default effect used to initialize attributes for this character, overriden in subclasses */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<class UGameplayEffect> DefaultAttributeSetterEffect;
	

	/** The GAS Component responsible for all gameplay actions of this character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	class UGASComponent* AbilitySystemComponent;

	/** Default attributes providing getters and setters for property bindings */
	UPROPERTY()
	class UGASAttributeSet* Attributes;

	/** Component providing access to runtime generated navmeshes */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UNavigationInvokerComponent* NavigationInvokerComponent;
	
	
	
	
};
