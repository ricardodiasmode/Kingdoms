// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatusBar.generated.h"

/**
 * Base class that holds info about life and mana of characters
 */
UCLASS()
class KINGDOMS_API UStatusBar : public UUserWidget
{
	GENERATED_BODY()
	
private:
	// [0, 1] variable
	float CurrentLife;
	float CurrentMana;

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentLife() const { return CurrentLife; }
	void SetCurrentLife(float LifeToSet) { CurrentLife = LifeToSet; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentMana() const { return CurrentMana; }
	void SetCurrentMana(float ManaToSet) { CurrentMana = ManaToSet; }

};
