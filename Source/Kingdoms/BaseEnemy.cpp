// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseEnemy.h"

#include "EnemyAIController.h"
#include "Kismet/KismetMathLibrary.h"
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
		GetWorldTimerManager().SetTimer(MoveTimer, this, &ABaseEnemy::MoveToCharacterDirection, 1.f, true, 0.0f);
}

void ABaseEnemy::MoveToCharacterDirection()
{
	// Making the calculations to know what quad should the AI move
	if (CharacterToChase)
		QuadToMove = FindWayToCharacterOrQuad(CharacterToChase->GetActorLocation(), true);
	else
		return;

	// Check if there is a way to character
	if (QuadToMove == FVector(0, 0, -9999))
		return;
	
	// Actually moving to the desired quad
	if (AAIController* MyAIController = Cast<AAIController>(GetController()))
	{
		if (AEnemyAIController* MyController = Cast< AEnemyAIController >(MyAIController))
			MyController->MoveToLocation(QuadToMove, 5.f, false, true, true, false, NULL, false);
	}
}

FVector ABaseEnemy::MoveDownUp(FVector LastQuadLocation, FVector FrontVector, bool TargetIsCharacter)
{
	#pragma region Init Variables
	FVector AddVectorUp(100.f, 0.f, 0.f);
	FVector AddVectorRight(0.f, 100.f, 0.f);
	FVector UpLocation = LastQuadLocation + AddVectorUp;
	FVector DownLocation = LastQuadLocation - AddVectorUp;
	FVector RightLocation = LastQuadLocation + AddVectorRight;
	FVector LeftLocation = LastQuadLocation - AddVectorRight;

	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, FCollisionQueryParams::GetUnknownStatId());
	FVector StartLocation = GetActorLocation();
	Params.AddIgnoredActor(this);

	UWorld* World = GetWorld();
	#pragma endregion

	// First try front
	if (FrontVector.X < 0)
	{
		if (!(World->LineTraceSingleByChannel(HitResult, StartLocation, UpLocation, ECC_Visibility, Params)))
		{
			//FVector MyLocation = GetActorLocation();
			//UE_LOG(LogTemp, Warning, TEXT("Up Location: %s xx My Location: %s"), *(UpLocation.ToString()), *(MyLocation.ToString()));
			//return FVector(0, 0, -9999);
			return FindWayToCharacterOrQuad(UpLocation, TargetIsCharacter);
		}
	}
	else
	{
		if (!(World->LineTraceSingleByChannel(HitResult, StartLocation, DownLocation, ECC_Visibility, Params)))
		{
			//FVector MyLocation = GetActorLocation();
			//UE_LOG(LogTemp, Warning, TEXT("Down Location: %s xx My Location: %s"), *(DownLocation.ToString()), *(MyLocation.ToString()));
			//return FVector(0, 0, -9999);
			return FindWayToCharacterOrQuad(DownLocation, TargetIsCharacter);
		}
	}
	// If front fails, try sides.
	if (FrontVector.Y < 0)
	{
		// If right is the fastest way, try right
		if (!(World->LineTraceSingleByChannel(HitResult, StartLocation, RightLocation, ECC_Visibility, Params)))
		{
			//FVector MyLocation = GetActorLocation();
			//UE_LOG(LogTemp, Warning, TEXT("RightLocation: %s xx My Location: %s"), *(RightLocation.ToString()), *(MyLocation.ToString()));
			//return FVector(0, 0, -9999);
			return FindWayToCharacterOrQuad(RightLocation, TargetIsCharacter);
		}
		// Right is blocked so try left
		else if (!(World->LineTraceSingleByChannel(HitResult, StartLocation, LeftLocation, ECC_Visibility, Params)))
		{
			//FVector MyLocation = GetActorLocation();
			//UE_LOG(LogTemp, Warning, TEXT("LeftLocation: %s xx My Location: %s"), *(LeftLocation.ToString()), *(MyLocation.ToString()));
			//return FVector(0, 0, -9999);
			return FindWayToCharacterOrQuad(LeftLocation, TargetIsCharacter);
		}
	}
	else
	{
		// If left is the fastest way, try left
		if (!(World->LineTraceSingleByChannel(HitResult, StartLocation, LeftLocation, ECC_Visibility, Params)))
		{
			//FVector MyLocation = GetActorLocation();
			//UE_LOG(LogTemp, Warning, TEXT("LeftLocation: %s xx My Location: %s"), *(LeftLocation.ToString()), *(MyLocation.ToString()));
			//return FVector(0, 0, -9999);
			return FindWayToCharacterOrQuad(LeftLocation, TargetIsCharacter);
		}
		// Left is blocked so try right
		if (!(World->LineTraceSingleByChannel(HitResult, StartLocation, RightLocation, ECC_Visibility, Params)))
		{
			//FVector MyLocation = GetActorLocation();
			//UE_LOG(LogTemp, Warning, TEXT("RightLocation: %s xx My Location: %s"), *(RightLocation.ToString()), *(MyLocation.ToString()));
			//return FVector(0, 0, -9999);
			return FindWayToCharacterOrQuad(RightLocation, TargetIsCharacter);
		}
	}
	// Now if front and sides are blocked, try back
	if (FrontVector.X > 0)
	{
		if (!(World->LineTraceSingleByChannel(HitResult, StartLocation, UpLocation, ECC_Visibility, Params)))
		{
			//FVector MyLocation = GetActorLocation();
			//UE_LOG(LogTemp, Warning, TEXT("Up Location: %s xx My Location: %s"), *(UpLocation.ToString()), *(MyLocation.ToString()));
			//return FVector(0, 0, -9999);
			return FindWayToCharacterOrQuad(UpLocation, TargetIsCharacter);
		}
	}
	else
	{
		if (!(World->LineTraceSingleByChannel(HitResult, StartLocation, DownLocation, ECC_Visibility, Params)))
		{
			//FVector MyLocation = GetActorLocation();
			//UE_LOG(LogTemp, Warning, TEXT("Down Location: %s xx My Location: %s"), *(DownLocation.ToString()), *(MyLocation.ToString()));
			//return FVector(0, 0, -9999);
			return FindWayToCharacterOrQuad(DownLocation, TargetIsCharacter);
		}
	}
	// If everything fails return null
	return FVector(0, 0, -9999);
}

FVector ABaseEnemy::MoveLeftRight(FVector LastQuadLocation, FVector FrontVector, bool TargetIsCharacter)
{
	#pragma region Init Variables
	FVector AddVectorUp(100.f, 0.f, 0.f);
	FVector AddVectorRight(0.f, 100.f, 0.f);
	FVector UpLocation = LastQuadLocation + AddVectorUp;
	FVector DownLocation = LastQuadLocation - AddVectorUp;
	FVector RightLocation = LastQuadLocation + AddVectorRight;
	FVector LeftLocation = LastQuadLocation - AddVectorRight;

	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, FCollisionQueryParams::GetUnknownStatId());
	FVector StartLocation = GetActorLocation();
	Params.AddIgnoredActor(this);

	UWorld* World = GetWorld();
	#pragma endregion

	// First try front
	if (FrontVector.Y < 0)
	{
		if (!(World->LineTraceSingleByChannel(HitResult, StartLocation, RightLocation, ECC_Visibility, Params)))
		{
			//FVector MyLocation = GetActorLocation();
			//return FVector(0, 0, -9999);
			return FindWayToCharacterOrQuad(RightLocation, TargetIsCharacter);
		}
	}
	else
	{
		if (!(World->LineTraceSingleByChannel(HitResult, StartLocation, LeftLocation, ECC_Visibility, Params)))
		{
			//FVector MyLocation = GetActorLocation();
			//UE_LOG(LogTemp, Warning, TEXT("LeftLocation: %s xx My Location: %s"), *(LeftLocation.ToString()), *(MyLocation.ToString()));
			//return FVector(0, 0, -9999);
			return FindWayToCharacterOrQuad(LeftLocation, TargetIsCharacter);
		}
	}
	// If front fails, try sides
	if (FrontVector.X < 0)
	{
		// If up is the fastest way, try up
		if (!(World->LineTraceSingleByChannel(HitResult, StartLocation, UpLocation, ECC_Visibility, Params)))
		{
			//FVector MyLocation = GetActorLocation();
			//UE_LOG(LogTemp, Warning, TEXT("UpLocation: %s xx My Location: %s"), *(UpLocation.ToString()), *(MyLocation.ToString()));
			//return FVector(0, 0, -9999);
			return FindWayToCharacterOrQuad(UpLocation, TargetIsCharacter);
		}
		// up is blocked so try down
		else if (!(World->LineTraceSingleByChannel(HitResult, StartLocation, DownLocation, ECC_Visibility, Params)))
		{
			//FVector MyLocation = GetActorLocation();
			//UE_LOG(LogTemp, Warning, TEXT("DownLocation: %s xx My Location: %s"), *(DownLocation.ToString()), *(MyLocation.ToString()));
			//return FVector(0, 0, -9999);
			return FindWayToCharacterOrQuad(DownLocation, TargetIsCharacter);
		}
	}
	else
	{
		// If down is the fastest way, try down
		if (!(World->LineTraceSingleByChannel(HitResult, StartLocation, DownLocation, ECC_Visibility, Params)))
		{
			//FVector MyLocation = GetActorLocation();
			//UE_LOG(LogTemp, Warning, TEXT("DownLocation: %s xx My Location: %s"), *(DownLocation.ToString()), *(MyLocation.ToString()));
			//return FVector(0, 0, -9999);
			return FindWayToCharacterOrQuad(DownLocation, TargetIsCharacter);
		}
		// Down is blocked so try up
		if (!(World->LineTraceSingleByChannel(HitResult, StartLocation, UpLocation, ECC_Visibility, Params)))
		{
			//FVector MyLocation = GetActorLocation();
			//UE_LOG(LogTemp, Warning, TEXT("UpLocation: %s xx My Location: %s"), *(UpLocation.ToString()), *(MyLocation.ToString()));
			//return FVector(0, 0, -9999);
			return FindWayToCharacterOrQuad(UpLocation, TargetIsCharacter);
		}
	}
	// Now if front and sides are blocked, try back
	if (FrontVector.Y > 0)
	{
		if (!(World->LineTraceSingleByChannel(HitResult, StartLocation, LeftLocation, ECC_Visibility, Params)))
		{
			//FVector MyLocation = GetActorLocation();
			//UE_LOG(LogTemp, Warning, TEXT("LeftLocation: %s xx My Location: %s"), *(LeftLocation.ToString()), *(MyLocation.ToString()));
			//return FVector(0, 0, -9999);
			return FindWayToCharacterOrQuad(LeftLocation, TargetIsCharacter);
		}
	}
	else
	{
		if (!(World->LineTraceSingleByChannel(HitResult, StartLocation, RightLocation, ECC_Visibility, Params)))
		{
			//FVector MyLocation = GetActorLocation();
			//UE_LOG(LogTemp, Warning, TEXT("RightLocation: %s xx My Location: %s"), *(RightLocation.ToString()), *(MyLocation.ToString()));
			//return FVector(0, 0, -9999);
			return FindWayToCharacterOrQuad(RightLocation, TargetIsCharacter);
		}
	}
	// If everything fails return null
	return FVector(0, 0, -9999);
}

FVector ABaseEnemy::FindWayToCharacterOrQuad(FVector LastQuadLocation, bool TargetIsCharacter)
{
	UE_LOG(LogTemp, Warning, TEXT("LastQuadLocation: %s xx My Location: %s xx CharacterLocation: %s"), *(LastQuadLocation.ToString()), *(GetActorLocation().ToString()), *(CharacterToChase->GetActorLocation().ToString()));
	
	#pragma region Check if is not too far and check the stop condition
	// Checking if i still close to character(it means i do not ran away tring to reach him) and if i ain't return NULL
	if (TargetIsCharacter)
	{
		if (CharacterToChase)
		{
			if (FVector(CharacterToChase->GetActorLocation() - LastQuadLocation).Size() > 3500 || FVector(CharacterToChase->GetActorLocation() - GetActorLocation()).Size() > 3500.f)
				return FVector(0, 0, -9999);
		}
	}
	// Checking if the current location is a quad by my side which means that finally found the quad
	FVector AddVectorUp(100.0, 0.0, 0.0);
	FVector AddVectorRight(0.0, 100.0, 0.0);
	FVector UpLocation = LastQuadLocation + AddVectorUp;
	FVector DownLocation = LastQuadLocation - AddVectorUp;
	FVector RightLocation = LastQuadLocation + AddVectorRight;
	FVector LeftLocation = LastQuadLocation - AddVectorRight;
		// Going up
	if (UpLocation.Equals(GetActorLocation(), 50.0) ||
		// Going down
		DownLocation.Equals(GetActorLocation(), 50.0) ||
		// Going right
		RightLocation.Equals(GetActorLocation(), 50.0) ||
		// Going left
		LeftLocation.Equals(GetActorLocation(), 50.0))
		return LastQuadLocation;

	// Check which side is front
	FVector FrontVector = LastQuadLocation - GetActorLocation();

	#pragma endregion

	// Priorizing front run the function again to reach the next quad
	if (FrontVector.X <= FrontVector.Y)
	{
		// Check se já está no X correto
		if(UKismetMathLibrary::Abs(LastQuadLocation.X - GetActorLocation().X) > 150.0)
			return MoveDownUp(LastQuadLocation, FrontVector, TargetIsCharacter);
		else
			return MoveLeftRight(LastQuadLocation, FrontVector, TargetIsCharacter);
	}
	else
	{
		// Check se já está no Y correto
		if (UKismetMathLibrary::Abs(LastQuadLocation.Y - GetActorLocation().Y) > 150.0)
			return MoveLeftRight(LastQuadLocation, FrontVector, TargetIsCharacter);
		else
			return MoveDownUp(LastQuadLocation, FrontVector, TargetIsCharacter);
	}
	return FVector(0, 0, -9999);
}

void ABaseEnemy::MoveToRandomQuad()
{
	if (GEngine)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			FHitResult HitResult;
			FCollisionQueryParams Params(NAME_None, FCollisionQueryParams::GetUnknownStatId());
			FVector StartLocation = GetActorLocation();
			Params.AddIgnoredActor(this);
			int RandomInteger = FMath::RandRange(0, 7);
			FVector LocationToMove;
			// Move up
			if (RandomInteger >= 6)
			{
				LocationToMove = FVector(GetActorLocation().X + 100, GetActorLocation().Y, GetActorLocation().Z);
				if (!(World->LineTraceSingleByChannel(HitResult, StartLocation, LocationToMove, ECC_Visibility, Params)))
				{
					if (AAIController* MyAIController = Cast<AAIController>(GetController()))
					{
						if (AEnemyAIController* MyController = Cast< AEnemyAIController >(MyAIController))
							MyController->MoveToLocation(LocationToMove, 5.f, false, true, true, false, NULL, false);
					}
				}
			}
			if (RandomInteger >= 4)
			{
				LocationToMove = FVector(GetActorLocation().X - 100, GetActorLocation().Y, GetActorLocation().Z);
				if (!(World->LineTraceSingleByChannel(HitResult, StartLocation, LocationToMove, ECC_Visibility, Params)))
				{
					if (AAIController* MyAIController = Cast<AAIController>(GetController()))
					{
						if (AEnemyAIController* MyController = Cast< AEnemyAIController >(MyAIController))
							MyController->MoveToLocation(LocationToMove, 5.f, false, true, true, false, NULL, false);
					}
				}
			}
			if (RandomInteger >= 2)
			{
				LocationToMove = FVector(GetActorLocation().X, GetActorLocation().Y + 100, GetActorLocation().Z);
				if (!(World->LineTraceSingleByChannel(HitResult, StartLocation, LocationToMove, ECC_Visibility, Params)))
				{
					if (AAIController* MyAIController = Cast<AAIController>(GetController()))
					{
						if (AEnemyAIController* MyController = Cast< AEnemyAIController >(MyAIController))
							MyController->MoveToLocation(LocationToMove, 5.f, false, true, true, false, NULL, false);
					}
				}
			}
			else
			{
				LocationToMove = FVector(GetActorLocation().X, GetActorLocation().Y - 100, GetActorLocation().Z);
				if (!(World->LineTraceSingleByChannel(HitResult, StartLocation, LocationToMove, ECC_Visibility, Params)))
				{
					if (AAIController* MyAIController = Cast<AAIController>(GetController()))
					{
						if (AEnemyAIController* MyController = Cast< AEnemyAIController >(MyAIController))
							MyController->MoveToLocation(LocationToMove, 5.f, false, true, true, false, NULL, false);
					}
				}
			}
		}
		else
		{
			return;
		}
	}
}

void ABaseEnemy::GoBackToSpawn()
{
	if (!(GetWorldTimerManager().IsTimerActive(MoveTimer)))
		GetWorldTimerManager().SetTimer(MoveTimer, this, &ABaseEnemy::MoveToSpawnDirection, 1.f, true, 0.0f);
	else
	{
		GetWorldTimerManager().ClearTimer(MoveTimer);
		GetWorldTimerManager().SetTimer(MoveTimer, this, &ABaseEnemy::MoveToSpawnDirection, 1.f, true, 0.0f);
	}
}

void ABaseEnemy::MoveToSpawnDirection()
{
	// Making the calculations to know what quad should the AI move
	QuadToMove = FindWayToCharacterOrQuad(InitialSpawnLocation, true);
	// Check if there is a way to character
	if (QuadToMove == FVector(0, 0, -9999))
		return;

	// Actually moving to the desired quad
	if (GetController())
	{
		if (AEnemyAIController* MyController = Cast< AEnemyAIController >(GetController()))
			MyController->MoveToLocation(QuadToMove, 5.f, false, true, true, false, NULL, false);
	}
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
