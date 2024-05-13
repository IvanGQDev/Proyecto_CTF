 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AGCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class USkeletalMeshComponent;

UCLASS()
class ABYSSALGUARDIANS_API AAGCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AAGCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

	//Line trace Custom Code
	UFUNCTION(BlueprintCallable, Category="Interaction")
	void PerformLineTrace();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* ThirdPersonSkeleton;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* FirstPersonSkeleton;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* CharacterMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* EquipAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Equip(const FInputActionValue& Value);

	//Line trace Custom Code
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction")
	float TraceDistance = 300.0f;

private:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(Replicatedusing = OnRep_OverlappingWeapon)
	class AAGWeapon* OverlappingWeapon;

	// On_Rep only works for clients since the direction is Server->Client so server is never notified
	UFUNCTION()
	void OnRep_OverlappingWeapon(AAGWeapon* LastWeapon);

	UPROPERTY(VisibleAnywhere)
	class UAGCombatComponent* CombatComponent;

	
	

	
public:
	 void SetOverlappingWeapon(AAGWeapon* Weapon);

};
