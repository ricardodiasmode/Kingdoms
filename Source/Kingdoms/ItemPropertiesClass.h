// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemPropertiesClass.generated.h"

UENUM(BlueprintType)
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
struct FS_ItemDrop
{
	GENERATED_USTRUCT_BODY()

	// Probability to drop (0 to 1)
	UPROPERTY(EditAnywhere)
	float ProbabilityToDrop = 0.f;
	UPROPERTY(EditAnywhere)
	int ItemIDToDrop;
};

USTRUCT(Blueprintable)
struct FItemProperties
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ItemCategory Category = ItemCategory::NotDefined;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ItemId = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* ItemMesh;
};
