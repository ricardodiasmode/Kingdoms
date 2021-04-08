// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Networking.h"
#include "Net/UnrealNetwork.h"
#include "ItemPropertiesClass.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickableItem.generated.h"

UCLASS()
class KINGDOMS_API APickableItem : public AActor
{
	GENERATED_BODY()

//// Variables ////
public:
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* MeshComponent;
private:
	UPROPERTY(ReplicatedUsing = OnRep_ItemID)
	int ItemID = -1;

//// Methods ////
public:	
	// Sets default values for this actor's properties
	APickableItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Function that calls set pickable properties
	UFUNCTION()
	void OnRep_ItemID();

	// Function that change pickable mesh
	UFUNCTION(BlueprintImplementableEvent)
	void OnSetId(int ItemIdSetted);
	UFUNCTION(BlueprintCallable)
	void SetPickableProperties(FItemProperties ItemProperties);
	UFUNCTION(Server, reliable)
	void Server_SetMyMesh(UStaticMesh* MeshToSet);
	UFUNCTION(NetMulticast, reliable)
	void Multicast_SetMyMesh(UStaticMesh* MeshToSet);

	void SetItemID(int IdToSet) { ItemID = IdToSet; }

};
