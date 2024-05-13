// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AGCombatComponent.generated.h"

class AAGWEapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ABYSSALGUARDIANS_API UAGCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAGCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	friend class AAGCharacter;

	void EquipWeapon(AAGWEapon* WeaponToEquip);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	class AAGCharacter* Character;
	AAGWEapon* Weapon;
public:	

		
};
