// Copyright Epic Games, Inc. All Rights Reserved.

#include "Online_FPGameMode.h"
#include "Online_FPCharacter.h"
#include "GameFramework/GameStateBase.h"
#include "UObject/ConstructorHelpers.h"

AOnline_FPGameMode::AOnline_FPGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}


