// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ExperienceBar.generated.h"

/**
 *  Base class that holds info about experience of characters
 */
UCLASS()
class KINGDOMS_API UExperienceBar : public UUserWidget
{
	GENERATED_BODY()

private:
	float CurrentExperience;
	int CurrentLevel;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnSetExp();

	UFUNCTION(BlueprintImplementableEvent)
	void OnSetLvl();

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentLevel() const { return CurrentLevel; }
	void SetCurrentLevel(float LevelToSet) { CurrentLevel = LevelToSet; OnSetLvl(); }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentExperience() const { return CurrentExperience; }
	void SetCurrentExperience(float ExperienceToSet) { CurrentExperience = ExperienceToSet; OnSetExp(); }
};
