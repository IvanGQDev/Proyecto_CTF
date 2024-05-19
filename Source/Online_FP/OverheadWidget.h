// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheadWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONLINE_FP_API UOverheadWidget : public UUserWidget
{
	GENERATED_BODY()
	public:
		UPROPERTY(meta=(BindWidget))
		class UTextBlock* DisplayText;
	    UPROPERTY(meta=(BindWidget))
		class UTextBlock* TextName;
		// can call the function
		void SetDisplayText(FString TextToDisplay, FString TextN);
		UFUNCTION(Blueprintable,BlueprintCallable)
		void ShowPlayerNetRole(APawn* InPawn);

	
	protected:
		virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld);
	
};
