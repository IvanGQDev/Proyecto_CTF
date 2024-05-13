// Fill out your copyright notice in the Description page of Project Settings.


#include "AGAnimInstance.h"
#include "AGCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UAGAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	AGCharacter = Cast<AAGCharacter>(TryGetPawnOwner());
}

void UAGAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (AGCharacter == nullptr)
	{
		AGCharacter = Cast<AAGCharacter>(TryGetPawnOwner());
	}
	if (AGCharacter == nullptr) return;

	FVector Velocity = AGCharacter->GetVelocity();
	Velocity.Z = 0.0f;
	Speed = Velocity.Size();

	bIsInAir = AGCharacter->GetCharacterMovement()->IsFalling();

	bIsAccelerating = AGCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f ? true : false;
}
