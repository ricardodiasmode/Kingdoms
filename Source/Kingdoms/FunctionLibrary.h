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

UENUM()
enum class ItemCategory : uint8 {
	NotDefined = 0 UMETA(DisplayName = "NotDefined"),
	Weapon = 1 UMETA(DisplayName = "Weapon"),
	Shield = 2 UMETA(DisplayName = "Shield"),
	Helmet = 3 UMETA(DisplayName = "Helmet"),
	Chest = 4 UMETA(DisplayName = "Chest"),
	Legs = 5 UMETA(DisplayName = "Legs"),
	Boots = 6 UMETA(DisplayName = "Boots"),
	Consumable = 7 UMETA(DisplayName = "Consumable"),
	Other = 8 UMETA(DisplayName = "Other"),
};

USTRUCT(BlueprintType)
struct FS_Item
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Name;

	UPROPERTY(BlueprintReadOnly)
	UTexture2D* Icon;

	UPROPERTY(BlueprintReadOnly)
	ItemCategory Category = ItemCategory::NotDefined;

	UPROPERTY(BlueprintReadOnly)
	int ItemId = 0;
};


UCLASS()
class KINGDOMS_API UFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

};
