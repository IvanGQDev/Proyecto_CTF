// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "Online_FPGameMode.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "interfaces/OnlineSessionInterface.h"
#include "Online_FPCharacter.generated.h"



class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AOnline_FPCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	public:
		AOnline_FPCharacter();
		

	protected:

		/** Called for movement input */
		void Move(const FInputActionValue& Value);

		/** Called for looking input */
		void Look(const FInputActionValue& Value);
				

	protected:
		// APawn interface
		virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
		
		// To add mapping context
		virtual void BeginPlay();

	public:
		/** Returns CameraBoom subobject **/
		FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
		/** Returns FollowCamera subobject **/
		FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

		// variable to assing the Connection to the online subsystem interface
	public:
		IOnlineSessionPtr OnlineSessionInterface;

	// Function for crate session
	protected:
		UFUNCTION(BlueprintCallable)
		void CreateGameSession();
	
		UFUNCTION(BlueprintCallable)
		void JoinGameSession();

		// Delegates answer
		void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
		void OnFindSessionsComplete(bool bWasSuccessful);
		void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	
	// Delegate variable for the callback, can say if the session was successful created, searched and joined
	private:
		FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
		FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
		FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
		TSharedPtr<FOnlineSessionSearch>SessionSearch;

		UPROPERTY(EditAnywhere,BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
		class UWidgetComponent* OverheadWidget;
	
	
	
};

