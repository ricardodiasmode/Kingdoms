// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseEnemy.h"

#include "EnemyAIController.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Creating widget component that holds mana and life
	StatusWidget = CreateDefaultSubobject<UWidgetComponent>("StatusBar");
	StatusWidget->SetupAttachment(RootComponent);
	static ConstructorHelpers::FClassFinder<UUserWidget> StatusWidgetClass(TEXT("/Game/UI/BP_StatusBar"));
	if (StatusWidgetClass.Class != nullptr)
	{
		StatusWidget->SetWidgetClass(StatusWidgetClass.Class);
	}
}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	CurrentLife = MaxLife;
	CurrentMana = MaxMana;
	StatusWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 110.0f));
	StatusWidgetRef = Cast< UStatusBar >(StatusWidget->GetUserWidgetObject());
}

// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (StatusWidgetRef)
		StatusWidgetRef->SetCurrentLife(GetCurrentLife() / MaxLife);
}

// Called to bind functionality to input
void ABaseEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABaseEnemy::SetTimerToChasePlayer(AKingdomsCharacter* CharacterRef)
{
	CharacterToChase = CharacterRef;
	if (!(GetWorldTimerManager().IsTimerActive(MoveTimer)))
		GetWorldTimerManager().SetTimer(MoveTimer, this, &ABaseEnemy::MoveToQuad, 1.f, true, 0.0f);
}

void ABaseEnemy::MoveToQuad()
{
	// Making the calculations to know what quad should the AI move
	QuadToMove = FindWayToCharacter(CharacterToChase->GetActorLocation());
	// Check if there is a way to character
	if (QuadToMove == NULL)
		return;

	// Actually moving to the desired quad
	if(GetController())
		if(AEnemyAIController* MyController = Cast< AEnemyAIController >(GetController()))
			MyController->MoveToLocation(QuadToMove, 5.f, false, true, true, false, NULL, false);
}

FVector ABaseEnemy::FindWayToCharacter(FVector LastQuadLocation)
{
	if (!(GEngine->GetWorld()))
		return NULL;
	// TO DO: Checking if i still close to character(it means i do not ran away tring to reach him) and if i ain't return NULL
	if (FVector(CharacterToChase->GetActorLocation() - GetActorLocation).Size() > 2000)
		return NULL;
	FVector AddVectorUp(100.f, 0.f, 0.f);
	FVector AddVectorRight(0.f, 100.f, 0.f);
	FVector UpLocation = LastQuadLocation + AddVectorUp;
	FVector DownLocation = LastQuadLocation - AddVectorUp;
	FVector RightLocation = LastQuadLocation + AddVectorRight;
	FVector LeftLocation = LastQuadLocation - AddVectorRight;
	
	/* Stop condition
	* Checking if the current location is a quad by my side which means that finally found the quad
	*/  // Going up
	if (UpLocation == GetActorLocation() ||
		// Going down
		DownLocation == GetActorLocation() ||
		// Going right
		RightLocation == GetActorLocation() ||
		// Going left
		LeftLocation == GetActorLocation())
		return LastQuadLocation;
	// Check which side is front
	FVector FrontVector = CharacterToChase->GetActorLocation() - GetActorLocation;
	// Checking which quad is avaliable PRIORIZING FRONT AND SIDES and run the function again to reach the next quad
	if (FrontVector.X > 0)
	{
		if (!(UKismetSystemLibrary::BoxOverlapActors(GEngine->GetWorld(), UpLocation, FVector(50.f, 50.f, 50.f), FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_Visibility), NULL, NULL, NULL));
			return FindWayToCharacter(UpLocation);
	}
	else
	{
		if (!(UKismetSystemLibrary::BoxOverlapActors(GEngine->GetWorld(), DownLocation, FVector(50.f, 50.f, 50.f), FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_Visibility), NULL, NULL, NULL));
			return FindWayToCharacter(DownLocation);
	}
	// If the front location is not avaliable, try sides
	if (FrontVector.Y > 0)
	{
		// Try right location. If is not avaliable, try left location.
		if (!(UKismetSystemLibrary::BoxOverlapActors(GEngine->GetWorld(), RightLocation, FVector(50.f, 50.f, 50.f), FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_Visibility), NULL, NULL, NULL));
			return FindWayToCharacter(RightLocation);
		else
			if (!(UKismetSystemLibrary::BoxOverlapActors(GEngine->GetWorld(), LeftLocation, FVector(50.f, 50.f, 50.f), FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_Visibility), NULL, NULL, NULL));
				return FindWayToCharacter(LeftLocation);
	}
	else
	{
		// Try left location. If is not avaliable, try right location.
		if (!(UKismetSystemLibrary::BoxOverlapActors(GEngine->GetWorld(), LeftLocation, FVector(50.f, 50.f, 50.f), FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_Visibility), NULL, NULL, NULL));
			return FindWayToCharacter(LeftLocation);
		else
			if (!(UKismetSystemLibrary::BoxOverlapActors(GEngine->GetWorld(), RightLocation, FVector(50.f, 50.f, 50.f), FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_Visibility), NULL, NULL, NULL));
				return FindWayToCharacter(RightLocation);
	}
	if (FrontVector.X < 0)
		// After try all locations, try back location. If is not avaliable return null
		if (!(UKismetSystemLibrary::BoxOverlapActors(GEngine->GetWorld(), DownLocation, FVector(50.f, 50.f, 50.f), FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_Visibility), NULL, NULL, NULL));
			return FindWayToCharacter(DownLocation);
		else
			return NULL;
}

void ABaseEnemy::MoveToRandomQuad()
{
	int RandomInteger = RandRange(0, 7);
	// Move up
	if (RandomInteger >= 6)
	{
		// Check if quad is avaliable
	}
	if (RandomInteger >= 4)
	{

	}
	if (RandomInteger >= 2)
	{

	}
}

void ABaseEnemy::GoBackToSpawn()
{

}

bool ABaseEnemy::OnBeingClicked()
{
	AmISelected = (AmISelected) ? false : true;
	ChangeMaterials(AmISelected);
	return AmISelected;
}

void ABaseEnemy::ChangeMaterials(bool IsSelected)
{
	if (!GetMesh())
		return;
	if (IsSelected)
	{
		for (int i = 0; i < GetMesh()->GetNumMaterials(); i++)
		{
			GetMesh()->SetMaterial(i, SelectedMaterials[i]);
		}
	}
	else
	{
		for (int i = 0; i < GetMesh()->GetNumMaterials(); i++)
		{
			GetMesh()->SetMaterial(i, DefaultMaterials[i]);
		}
	}
}

void ABaseEnemy::LoseLife(const float Damage)
{
	SetCurrentLife(GetCurrentLife() - Damage);
	CheckShouldDie();
}

void ABaseEnemy::CheckShouldDie()
{
	if (GetCurrentLife() <= 0)
	{
		Server_DestroyMe();
	}
}

void ABaseEnemy::Server_DestroyMe_Implementation()
{
	Destroy();
}
