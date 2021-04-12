// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ItemPropertiesClass.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KINGDOMS_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

	//// Variables ////
private:
	TArray< FItemProperties > MyItems;


	//// Methods ////
	void AddItem(FItemProperties ItemToAdd) { MyItems.Add(ItemToAdd); }
	void AddItemToIndex(FItemProperties ItemToAdd, int IndexToAdd) { MyItems.Insert(ItemToAdd, IndexToAdd); }

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
