// Fill out your copyright notice in the Description page of Project Settings.


#include "AGCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "AbyssalGuardians/AGComponents/AGCombatComponent.h"
#include "AbyssalGuardians/Weapon/AGWeapon.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"

AAGCharacter::AAGCharacter()
{
 	
	PrimaryActorTick.bCanEverTick = true;

	ThirdPersonSkeleton = GetMesh();
	ThirdPersonSkeleton->SetOwnerNoSee(true);

	FirstPersonSkeleton = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPSkeleton"));
	FirstPersonSkeleton->SetupAttachment(GetRootComponent());
	FirstPersonSkeleton->SetOnlyOwnerSee(true);
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(FirstPersonSkeleton);
	CameraBoom->TargetArmLength = 600.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	CombatComponent = CreateDefaultSubobject<UAGCombatComponent>(TEXT("CombatComponent"));
	CombatComponent->SetIsReplicated(true);
	
}

void AAGCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Here Register MACROS
	DOREPLIFETIME_CONDITION(AAGCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void AAGCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(CharacterMappingContext, 0);
		}
	}
}
void AAGCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PerformLineTrace();

}


void AAGCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AAGCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	
	if (GetController())
	{
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void AAGCharacter::Equip(const FInputActionValue& Value)
{
	
}

void AAGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAGCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAGCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AAGCharacter::Jump);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &AAGCharacter::Equip);
	}
}

void AAGCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (CombatComponent)
	{
		CombatComponent->Character = this;
	}
}

//Line trace custom code
void AAGCharacter::PerformLineTrace()
{
	FVector EyeLocation;
	FRotator EyeRotation;
	GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector End = EyeLocation + (EyeRotation.Vector() * TraceDistance);
	FHitResult HitResult;

	// Set up the collision query parameters
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // Ignore the player character
	QueryParams.bTraceComplex = true;

	// Perform the line trace
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		EyeLocation,
		End,
		ECC_Visibility, // Change this as necessary for your game
		QueryParams
	);

	// Handle the hit
	if (bHit)
	{
		// You can access HitResult.Actor and other details here
		// Optionally draw debug line
		DrawDebugLine(GetWorld(), EyeLocation, End, FColor::Red, false, 1.0f, 0, 1.0f);

		if (HitResult.GetActor() != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitResult.GetActor()->GetName());
		}
	}
}


//This is the method for the server only to replicate weapon interaction
void AAGCharacter::SetOverlappingWeapon(AAGWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

//This is for the clients only to handle weapon interaction
void AAGCharacter::OnRep_OverlappingWeapon(AAGWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

