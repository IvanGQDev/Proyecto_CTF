// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"

#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UOverheadWidget::SetDisplayText(FString TextToDisplay, FString TextN)
{
	if(DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
	if(TextName)
	{
		TextName->SetText(FText::FromString(TextN));
	}
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	
	if (!InPawn)
	{
		SetDisplayText(TEXT("Remote role: Unknown"), TEXT("Name: Unknown"));
		return;
	}

	APlayerState* PlayerState = InPawn->GetPlayerState();
	if (!PlayerState)
	{
		SetDisplayText(TEXT("Remote role: Unknown"), TEXT("Name: Unknown"));
		return;
	}

	ENetRole RemoteRole = InPawn->GetLocalRole();
	FString Role;
	

	switch (RemoteRole)
	{
	case ROLE_Authority:
		Role = "Host";
		break;
	case ROLE_AutonomousProxy:
	case ROLE_SimulatedProxy:
		Role = "Client";
		break;
	case ROLE_None:
		Role = "None";
		break;
	default:
		Role = "Unknown";
		break;
	}

	FString RemoteRoleString = FString::Printf(TEXT("Remote role: %s"), *Role);
	FString PlayerName = PlayerState->GetPlayerName();  
	FString PlayerNameString = FString::Printf(TEXT("Name: %s"), *PlayerName);

	SetDisplayText(RemoteRoleString, PlayerNameString);
}

void UOverheadWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	RemoveFromParent();
	Super::NativeDestruct();

}
