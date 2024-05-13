// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AGOverheadWidget.generated.h"

/**
 * 
 */
UCLASS()
class ABYSSALGUARDIANS_API UAGOverheadWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DisplayText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DisplayName;

	void SetDisplayText(FString TextToDisplay);

	void SetDisplayName(FString NameToDisplay);

	UFUNCTION(BlueprintCallable)
	void ShowPlayerNetRole(APawn* InPawn);

	UFUNCTION(BlueprintCallable)
	void ShowPlayerName(APawn* InPawn);

protected:
	void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld);

private:
	FTimerHandle PlayerNameTimerHandle;
};
