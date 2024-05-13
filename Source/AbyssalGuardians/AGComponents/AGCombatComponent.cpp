// Fill out your copyright notice in the Description page of Project Settings.


#include "AGCombatComponent.h"
#include "AbyssalGuardians/Weapon/AGWeapon.h"


UAGCombatComponent::UAGCombatComponent()
{

	PrimaryComponentTick.bCanEverTick = false;

}



void UAGCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
	
}



void UAGCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

void UAGCombatComponent::EquipWeapon(AAGWEapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr) return;

	
	
}

