// Fill out your copyright notice in the Description page of Project Settings.


#include "AGLobbyGameMode.h"
#include "GameFramework/GameStateBase.h"


void AAGLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	if(NumberOfPlayers == 2)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			bUseSeamlessTravel = true;
			World->ServerTravel(FString("/Game/_Game/Maps/GameMap?listen"));
		}
			
	}
}
