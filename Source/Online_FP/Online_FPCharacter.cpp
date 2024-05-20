// Copyright Epic Games, Inc. All Rights Reserved.

#include "Online_FPCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Components/WidgetComponent.h"
#include "Online/OnlineSessionNames.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AOnline_FPCharacter

AOnline_FPCharacter::AOnline_FPCharacter():
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this,&ThisClass::OnCreateSessionComplete)),
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this,&ThisClass::OnFindSessionsComplete)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	// Connection with the online interface  and get the subsystem that we are using
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if(OnlineSubsystem)
	{
		OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();

		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Blue,
				FString::Printf(TEXT("Found subsystem %s"),*OnlineSubsystem->GetSubsystemName().ToString())
			);
		}
	}
	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);
}

void AOnline_FPCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

// Create session functionality
void AOnline_FPCharacter::CreateGameSession()
{
	if(!OnlineSessionInterface.IsValid())
	{
		return;
	}

	// We need destroy existing session to create a new session
	auto ExistingSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession);
	if(ExistingSession != nullptr)
	{
		OnlineSessionInterface->DestroySession(NAME_GameSession);
		
	}
	// Add The delegate to the list of the online session, and when the session will be created the delegate will be called
	OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	// Settings definition
	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings);
	SessionSettings->bIsLANMatch = false;
	SessionSettings->NumPublicConnections = 2;
	SessionSettings->bAllowJoinInProgress = true;
	// Allow the region search sessions
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bUsesPresence = true;
	SessionSettings->bUseLobbiesIfAvailable = true;
	// Set the type of game
	SessionSettings->Set(FName("MatchType"),FString("FreeForAll"),EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	// Get the id of the first player
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	// Create session, assign de id of the player , a namesesion, and the settings
	OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(),NAME_GameSession,*SessionSettings);
	
}

// 18, 19, 20 y 21 Folio: 21566

void AOnline_FPCharacter::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Blue,
				FString::Printf(TEXT("Created session: %s"),*SessionName.ToString())
			);
		}

		UWorld* World = GetWorld();
		if(World)
		{
			// The player will be trave to the lobby map and using ?listen, set the session as listener
			World->ServerTravel(FString("/Game/BigStarStation/Maps/Map_BigStarStation?listen"));
		}
	}
	else
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Red,
				FString(TEXT("Failed to create session"))
			);
		}
	}
}

void AOnline_FPCharacter::OnFindSessionsComplete(bool bWasSuccessful)
{
	if(!OnlineSessionInterface.IsValid())
	{
		return;
	}
	// Get the SearchResults
	for(auto Result : SessionSearch->SearchResults)
	{
		FString Id = Result.GetSessionIdStr();
		FString User = Result.Session.OwningUserName;
		FString MatchType;

		// Validate if the match type it´s the same
		Result.Session.SessionSettings.Get(FName("MatchType"),MatchType);
		
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Cyan,
				FString::Printf(TEXT("ID: %s, User: %s"),*Id,*User)
			);
		}

		if(MatchType == FString("FreeForAll"))
		{
			if(GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					15.f,
					FColor::Cyan,
					FString::Printf(TEXT("Joining match type: %s"),*MatchType)
				);
			}

			// Add our delegate to the interface list
			OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);
			
			const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession,Result);
		}
	}
}

// Delegate or callback for joinSession
void AOnline_FPCharacter::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if(!OnlineSessionInterface.IsValid())
	{
		return;
	}

	// Get ip add
	FString Address;
	if(OnlineSessionInterface->GetResolvedConnectString(NAME_GameSession,Address))
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Yellow,
				FString::Printf(TEXT("Connect: %s"),*Address)
			);
		}
		// Now we can join to the host lobby or session
		APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
		if(PlayerController)
		{
			PlayerController->ClientTravel(Address,TRAVEL_Absolute);
		}
	}
}

// Join game session functionality
void AOnline_FPCharacter::JoinGameSession()
{
	if(!OnlineSessionInterface.IsValid())
	{
		return;
	}

	// Add our delegate to the delegate list and when de search finish this delegate will be callback
	OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);
	
	// Find game sessions
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	// Set the max results that can find the user, in this case we don´t have our own id for the game and this id is public, we need set a big limit, because we can find other hots from other tester games
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->bIsLanQuery = false;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(),SessionSearch.ToSharedRef());
}

//////////////////////////////////////////////////////////////////////////
// Input

void AOnline_FPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AOnline_FPCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AOnline_FPCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AOnline_FPCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AOnline_FPCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

