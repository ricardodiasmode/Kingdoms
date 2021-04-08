// Fill out your copyright notice in the Description page of Project Settings.


#include "PickableItem.h"

#include "Kismet/KismetSystemLibrary.h"

void APickableItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APickableItem, ItemID);
}

// Sets default values
APickableItem::APickableItem()
{
	bReplicates = true;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setting the static mesh according to ItemProperties
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	RootComponent = MeshComponent;
}

// Called when the game starts or when spawned
void APickableItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickableItem::OnRep_ItemID()
{
	if(ItemID != -1)
		OnSetId(ItemID);
}

void APickableItem::SetPickableProperties(FItemProperties ItemProperties)
{
	Server_SetMyMesh_Implementation(ItemProperties.ItemMesh);
}


void APickableItem::Server_SetMyMesh_Implementation(UStaticMesh* MeshToSet)
{
	Multicast_SetMyMesh(MeshToSet);
}

void APickableItem::Multicast_SetMyMesh_Implementation(UStaticMesh* MeshToSet)
{
	if (MeshComponent)
	{
		if (MeshToSet)
		{
			MeshComponent->SetStaticMesh(MeshToSet);
		}
	}
}
