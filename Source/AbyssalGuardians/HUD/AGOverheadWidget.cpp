// Fill out your copyright notice in the Description page of Project Settings.


#include "AGOverheadWidget.h"

#include "AbyssalGuardians/Character/AGCharacter.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UAGOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UAGOverheadWidget::SetDisplayName(FString NameToDisplay)
{
	if (DisplayName)
	{
		DisplayName->SetText(FText::FromString(NameToDisplay));
	}
}

void UAGOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	ENetRole RemoteRole = InPawn->GetRemoteRole();
	FString Role;
	switch (RemoteRole)
	{
	case ENetRole::ROLE_Authority:
		Role = FString("Authority");
		break;
	case ENetRole::ROLE_AutonomousProxy:
		Role = FString("Autonomous Proxy");
		break;
	case ENetRole::ROLE_SimulatedProxy:
		Role = FString("Simulated Proxy");
		break;
	case ENetRole::ROLE_None:
		Role = FString("None");
		break;
	}
	FString RemoteRoleString = FString::Printf(TEXT("Remote Role: %s"), *Role);
	SetDisplayText(RemoteRoleString);
}

void UAGOverheadWidget::ShowPlayerName(APawn* InPawn)
{
	const APlayerState* PlayerState = InPawn->GetPlayerState();
	if (PlayerState)
	{
		FString PlayerName = FString::Printf(TEXT("Name: %s"), *PlayerState->GetPlayerName());
		SetDisplayName(PlayerName);

		// Clear the timer if PlayerState is obtained
		GetWorld()->GetTimerManager().ClearTimer(PlayerNameTimerHandle);
	}
	else if (!PlayerNameTimerHandle.IsValid())
	{
		// Start the timer if it's not already running
		GetWorld()->GetTimerManager().SetTimer(PlayerNameTimerHandle, [this, InPawn]() {
			ShowPlayerName(InPawn);
		}, 0.5f, true);
	}
	
	
}

void UAGOverheadWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	RemoveFromParent();
	Super::NativeDestruct();
	
	if (PlayerNameTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(PlayerNameTimerHandle);
	}
}

