// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FunctionLibrary.generated.h"

/**
 * Lybrary that holds part of gameplay logic
 */

UENUM()
enum class CharacterClass : uint8
{
	Civilian = 0 UMETA(DisplayName = "Civilian"),
	Knight = 1 UMETA(DisplayName = "Knight"),
	Mage = 2 UMETA(DisplayName = "Mage"),
	Archer = 3 UMETA(DisplayName = "Archer"),
	Priest = 4 UMETA(DisplayName = "Priest"),
};

UCLASS()
class KINGDOMS_API UFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

};
