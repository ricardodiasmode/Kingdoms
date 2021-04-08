// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ItemPropertiesClass.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterInventory.generated.h"

/*
* Inventory that only server can see and send info to client then he can update the inventory widget
*/
UCLASS()
class KINGDOMS_API ACharacterInventory : public AActor
{
	GENERATED_BODY()

//// Variables ////
private:
TArray< FItemProperties > MyItems;


//// Methods ////
void AddItem(FItemProperties ItemToAdd) { MyItems.Add(ItemToAdd); }
void AddItemToIndex(FItemProperties ItemToAdd, int IndexToAdd) { MyItems.Insert(ItemToAdd, IndexToAdd); }

//// Default ////

public:	
	// Sets default values for this actor's properties
	ACharacterInventory();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
