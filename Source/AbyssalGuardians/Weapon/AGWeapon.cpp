// Fill out your copyright notice in the Description page of Project Settings.


#include "AGWeapon.h"

#include "AbyssalGuardians/Character/AGCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"


AAGWeapon::AAGWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);
}

void AAGWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}

	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		/*AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);*/
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AAGWeapon::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AAGWeapon::OnSphereEndOverlap);
	}
}

void AAGWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAGWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AAGCharacter* AGCharacter = Cast<AAGCharacter>(OtherActor);
	if (AGCharacter && PickupWidget)
	{
		AGCharacter->SetOverlappingWeapon(this);
	}
	// -1 ensures the message doesn't overwrite any other messages, 5.0f is the duration, FColor::Red is the color
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("This is an on-screen debug message"));
	
}

void AAGWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AAGCharacter* AGCharacter = Cast<AAGCharacter>(OtherActor);
	if (AGCharacter && PickupWidget)
	{
		AGCharacter->SetOverlappingWeapon(nullptr);
	}
}

void AAGWeapon::ShowPickupWidget(bool bShowWidget)
{
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(bShowWidget);
	}
}




