// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WidgetSlotInventario.h"
#include "FunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetInventario.generated.h"

/**
 * Widget that holds interaction between player and inventory
 */
UCLASS()
class KINGDOMS_API UWidgetInventario : public UUserWidget
{
	GENERATED_BODY()

private:
	// Inventory slots
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TArray<UWidgetSlotInventario*> Slots;

public:
	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	int NumberOfSlots = 0;

	//// Functions ////
public:
	virtual void NativeConstruct() override;

	/* Function that creates all the slots */
	void GenerateSlots(int SlotsToCreate);

	/* Function that returns the item in the index. Returns null if there is no item.
	* @param SlotIndex: Aimed slot
	*/
	UFUNCTION(BlueprintCallable)
	FS_Item GetItemByIndex(int SlotIndex);

	/* Function that returns if has or not the desired item.
	* @param ItemID: Aimed slot
	*/
	UFUNCTION(BlueprintCallable)
	bool CheckHasItem(int ItemID);
};
