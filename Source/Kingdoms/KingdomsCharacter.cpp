// Copyright Epic Games, Inc. All Rights Reserved.

#include "KingdomsCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Engine/World.h"

#include "BaseEnemy.h"
#include "KingdomsPlayerController.h"
#include "Kismet/KismetMathLibrary.h"

void AKingdomsCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AKingdomsCharacter, IsMoving);
	DOREPLIFETIME(AKingdomsCharacter, CurrentExperience);
	DOREPLIFETIME(AKingdomsCharacter, RequiredExperienceToUp);
	DOREPLIFETIME(AKingdomsCharacter, CurrentLevel);
	DOREPLIFETIME(AKingdomsCharacter, CurrentLife);
	DOREPLIFETIME(AKingdomsCharacter, CurrentMana);
	DOREPLIFETIME(AKingdomsCharacter, MaxLife);
	DOREPLIFETIME(AKingdomsCharacter, MaxMana);
}

AKingdomsCharacter::AKingdomsCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create inventory
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));

	// Create a decal in the world to show the cursor's location
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/Character/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Creating widget component that holds mana and life
	StatusWidget = CreateDefaultSubobject<UWidgetComponent>("StatusBar");
	StatusWidget->SetupAttachment(RootComponent);
	static ConstructorHelpers::FClassFinder<UUserWidget> StatusWidgetClass(TEXT("/Game/UI/BP_StatusBar"));
	if (StatusWidgetClass.Class != nullptr)
	{
		StatusWidget->SetWidgetClass(StatusWidgetClass.Class);
	}

	// Creating widget component that holds experience
	ExperienceWidget = CreateDefaultSubobject<UWidgetComponent>("ExperienceBar");
	ExperienceWidget->SetupAttachment(RootComponent);
	static ConstructorHelpers::FClassFinder<UUserWidget> ExperienceWidgetClass(TEXT("/Game/UI/BP_ExperienceBar"));
	if (ExperienceWidgetClass.Class != nullptr)
	{
		ExperienceWidget->SetWidgetClass(ExperienceWidgetClass.Class);
	}
}

void AKingdomsCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Click", IE_Pressed, this, &AKingdomsCharacter::OnClick);
	PlayerInputComponent->BindAxis("MoveForward", this, &AKingdomsCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AKingdomsCharacter::MoveRight);
}

void AKingdomsCharacter::OnClick()
{
	if (IsLocallyControlled())
	{
		if (UWorld* World = GetWorld())
		{
			FHitResult HitResult;
			FCollisionQueryParams Params(NAME_None, FCollisionQueryParams::GetUnknownStatId());
			FVector StartLocation = TopDownCameraComponent->GetComponentLocation();
			FVector MouseWorldLocation;
			FVector MouseWorldDirection;
			if (AKingdomsPlayerController* MyController = Cast< AKingdomsPlayerController>(GetController()))
				MyController->DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection);
			else
				return;
			FVector EndLocation = MouseWorldLocation + MouseWorldDirection * 10000.f;
			Params.AddIgnoredActor(this);
			if (World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_GameTraceChannel1, Params))
			{

				if (ABaseEnemy* InimigoAtingido = Cast<ABaseEnemy>(HitResult.GetActor()))
				{
					if (InimigoAtingido->OnBeingClicked())
						Client_OnSelectEnemy(InimigoAtingido);
					else
						Client_OnDeselectEnemy();
				}
			}
		}
	}
}

#pragma region Move System

void AKingdomsCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// Check if character is in center of the quad
		if (((int)GetActorLocation().X % 100) < 1)
		{
			float AdditionalMove = 100 * Value;
			FVector LocalLocationToMove(GetActorLocation().X + AdditionalMove, GetActorLocation().Y, GetActorLocation().Z);
			Server_MoveWithMoveSpeed(LocalLocationToMove);
		}
		else
			AdjustQuad(true);
	}
}

void AKingdomsCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// Check if character is in center of the quad
		if (((int)GetActorLocation().Y % 100) < 1)
		{
			float AdditionalMove = 100 * Value;
			FVector LocalLocationToMove(GetActorLocation().X, GetActorLocation().Y + AdditionalMove, GetActorLocation().Z);
			Server_MoveWithMoveSpeed(LocalLocationToMove);
		}
		else
			AdjustQuad(false);
	}
}

void AKingdomsCharacter::AdjustQuad(bool IsX)
{
	if (IsMoving)
		return;
	if (IsX)
	{
		int Module = (int)GetActorLocation().X % 100;
		// Check if is closer to X+
		double integral;
		float fractional;
		fractional = modf(GetActorLocation().X / 100, &integral);
		if (fractional > 0.5 || (fractional > -0.5 && fractional < 0))
		{
			UE_LOG(LogTemp, Warning, TEXT("setting x+"));
			SetActorLocation(FVector(GetActorLocation().X + 0.25, GetActorLocation().Y, GetActorLocation().Z));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("setting x-"));
			SetActorLocation(FVector(GetActorLocation().X - 0.25, GetActorLocation().Y, GetActorLocation().Z));
		}
	}
	else
	{
		int Module = (int)GetActorLocation().Y % 100;
		// Check if is closer to Y+
		double integral;
		float fractional;
		fractional = modf(GetActorLocation().Y / 100, &integral);
		if (fractional > 0.5 || (fractional > -0.5 && fractional < 0))
		{
			UE_LOG(LogTemp, Warning, TEXT("setting y+"));
			SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y + 0.25, GetActorLocation().Z));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("setting y-"));
			SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y - 0.25, GetActorLocation().Z));
		}
	}
}

void AKingdomsCharacter::Server_MoveWithMoveSpeed_Implementation(const FVector Location)
{
	if (!IsMoving)
	{
		LocationToMove = Location;
		// This bool will tell tick event that character should move
		IsMoving = true;
		if (!(GetWorldTimerManager().IsTimerActive(MoveTimer)))
			GetWorldTimerManager().SetTimer(MoveTimer, this, &AKingdomsCharacter::CheckReachedDesiredLocation, 0.1f, true, 0.0f);
		
	}
}

void AKingdomsCharacter::CheckReachedDesiredLocation()
{
	if (LocationToMove.Equals(GetActorLocation(), 1.0))
	{
		IsMoving = false;
		if (GetWorldTimerManager().IsTimerActive(MoveTimer))
			GetWorldTimerManager().ClearTimer(MoveTimer);
	}
}

void AKingdomsCharacter::Multicast_AddMovementInput_Implementation(const FVector Direction)
{
	AddMovementInput(Direction, 0.025, false);
}

#pragma endregion

void AKingdomsCharacter::SetAttackRange()
{
	AttackRange = 1.f;
}

void AKingdomsCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		CurrentLife = MaxLife;
		OnRep_CurrentLife();
		CurrentMana = MaxMana;
		OnRep_CurrentMana();
		// Setting level and exp to default while we have no save
		CurrentLevel = 1;
		OnRep_CurrentLevel();
		CurrentExperience = 0;
		RequiredExperienceToUp = 1 + pow(CurrentLevel, 2);
		OnRep_RequiredExperienceToUp();
	}

	SetAttackRange();

	StatusWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
	StatusWidgetRef = Cast< UStatusBar >(StatusWidget->GetUserWidgetObject());

	ExperienceWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));
	ExperienceWidgetRef = Cast< UExperienceBar >(ExperienceWidget->GetUserWidgetObject());
}

void AKingdomsCharacter::AddExperience(int ExperienceToAdd)
{
	CurrentExperience += ExperienceToAdd;
	// Check if player reach the required experience to upgrade
	while (CurrentExperience >= RequiredExperienceToUp)
	{
		// Upgrading level
		CurrentExperience -= RequiredExperienceToUp;
		CurrentLevel++;
		OnRep_CurrentLevel();
		// Increase required experience to upgrade
		RequiredExperienceToUp = 1 + pow(CurrentLevel, 2);
		OnRep_RequiredExperienceToUp();
	}
}

void AKingdomsCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


	// Check if want to move then add movement input
	if (HasAuthority())
	{
		if (IsMoving)
		{
			Multicast_AddMovementInput(LocationToMove - GetActorLocation());
			AddMovementInput(LocationToMove - GetActorLocation(), 0.015, false);
		}
	}

	// Linetrace on cursor
	if (CursorToWorld != nullptr)
	{
		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
		{
			if (UWorld* World = GetWorld())
			{
				FHitResult HitResult;
				FCollisionQueryParams Params(NAME_None, FCollisionQueryParams::GetUnknownStatId());
				FVector StartLocation = TopDownCameraComponent->GetComponentLocation();
				FVector EndLocation = TopDownCameraComponent->GetComponentRotation().Vector() * 2000.0f;
				Params.AddIgnoredActor(this);
				if (World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params))
				{
					if (IsLocallyControlled())
					{
						if (HitResult.Actor)
						{
							// Check if the actor hovered is the pickable item
							if (Cast< APickableItem >(HitResult.Actor))
								Cast< APickableItem >(HitResult.Actor)->NameWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::HitTestInvisible)
						}
					}
				}
				FQuat SurfaceRotation = HitResult.ImpactNormal.ToOrientationRotator().Quaternion();
				CursorToWorld->SetWorldLocationAndRotation(HitResult.Location, SurfaceRotation);
			}
		}
		else if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			FHitResult TraceHitResult;
			PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			CursorToWorld->SetWorldLocation(TraceHitResult.Location);
			CursorToWorld->SetWorldRotation(CursorR);
		}
	}
}

void AKingdomsCharacter::Server_OnDeselectEnemy_Implementation()
{
	CurrentlyHittedEnemy = nullptr;
	if (!(GetWorldTimerManager().IsTimerActive(HitEnemyTimer)))
		GetWorldTimerManager().ClearTimer(HitEnemyTimer);
}

bool AKingdomsCharacter::Client_OnDeselectEnemy_Validate()
{
	if (CurrentlyHittedEnemy)
		return true;
	return false;
}

void AKingdomsCharacter::Client_OnDeselectEnemy_Implementation()
{
	Cast< ABaseEnemy>(CurrentlyHittedEnemy)->ChangeMaterials(false);
	Server_OnDeselectEnemy();
}

void AKingdomsCharacter::Client_OnSelectEnemy_Implementation(class ACharacter* EnemyToHit)
{
	if (CurrentlyHittedEnemy != nullptr)
		Client_OnDeselectEnemy();
	if (EnemyToHit != nullptr)
		CurrentlyHittedEnemy = EnemyToHit; 
	Cast< ABaseEnemy>(CurrentlyHittedEnemy)->ChangeMaterials(true);
	Server_OnSelectEnemy(CurrentlyHittedEnemy);
}

bool AKingdomsCharacter::Server_OnSelectEnemy_Validate(class ACharacter* EnemyToHit)
{
	if (EnemyToHit != nullptr)
		return true;
	return false;
}

void AKingdomsCharacter::Server_OnSelectEnemy_Implementation(class ACharacter* EnemyToHit)
{
	if (EnemyToHit != nullptr)
		CurrentlyHittedEnemy = EnemyToHit;
	// Timer that makes player hits based on attack speed
	if(!(GetWorldTimerManager().IsTimerActive(HitEnemyTimer)))
		GetWorldTimerManager().SetTimer(HitEnemyTimer, this, &AKingdomsCharacter::HitEnemy, GetCurrentAttackSpeed(), true, 0.0f);
}

void AKingdomsCharacter::HitEnemy()
{
	if (CurrentlyHittedEnemy)
	{
		ABaseEnemy* NPCToHit = Cast<ABaseEnemy>(CurrentlyHittedEnemy);
		if (NPCToHit)
		{
			// Checking if target is in range with a 50uu spare
			FVector DistanceToTarget = NPCToHit->GetActorLocation() - GetActorLocation();
			if(DistanceToTarget.Size() <= AttackRange*100 + 50)
				NPCToHit->LoseLife(GetCurrentBaseDamage(), this);
		}
	}
}

void AKingdomsCharacter::RecieveDamage(float DamageToRecieve)
{
	CurrentLife -= DamageToRecieve;
	OnRep_CurrentLife();
}

#pragma region OnRep Functions

void AKingdomsCharacter::OnRep_CurrentLife()
{
	// Setting current life on status widget
	if (StatusWidgetRef)
	{
		StatusWidgetRef->SetCurrentLife(GetCurrentLife() / MaxLife);
	}
	else
	{
		FTimerHandle UnusedHandle;
		GetWorldTimerManager().SetTimer(UnusedHandle, this, &AKingdomsCharacter::TrySetCurrentLifeAgain, 1.f, false);
	}
}

void AKingdomsCharacter::TrySetCurrentLifeAgain()
{
	if (StatusWidgetRef)
	{
		StatusWidgetRef->SetCurrentLife(GetCurrentLife() / MaxLife);
	}
}

void AKingdomsCharacter::OnRep_RequiredExperienceToUp()
{
	// Setting current exp on experience widget
	if (ExperienceWidgetRef)
	{
		ExperienceWidgetRef->SetCurrentExperience((float)CurrentExperience / (float)RequiredExperienceToUp);
	}
	else
	{
		FTimerHandle UnusedHandle;
		GetWorldTimerManager().SetTimer(UnusedHandle, this, &AKingdomsCharacter::TrySetRequiredExperienceToUpAgain, 1.f, false);
	}
}

void AKingdomsCharacter::TrySetRequiredExperienceToUpAgain()
{
	if (ExperienceWidgetRef)
	{
		ExperienceWidgetRef->SetCurrentExperience((float)CurrentExperience / (float)RequiredExperienceToUp);
	}
}

void AKingdomsCharacter::OnRep_CurrentLevel()
{
	// Setting current exp on experience widget
	if (ExperienceWidgetRef)
	{
		ExperienceWidgetRef->SetCurrentLevel(CurrentLevel);
	}
	else
	{
		FTimerHandle UnusedHandle;
		GetWorldTimerManager().SetTimer(UnusedHandle, this, &AKingdomsCharacter::TrySetCurrentLevelAgain, 1.f, false);
	}
}

void AKingdomsCharacter::TrySetCurrentLevelAgain()
{
	if (ExperienceWidgetRef)
	{
		ExperienceWidgetRef->SetCurrentLevel(CurrentLevel);
	}
}

void AKingdomsCharacter::OnRep_CurrentMana()
{
	// Setting current exp on experience widget
	if (StatusWidgetRef)
	{
		StatusWidgetRef->SetCurrentMana(GetCurrentMana() / MaxMana);
	}
	else
	{
		FTimerHandle UnusedHandle;
		GetWorldTimerManager().SetTimer(UnusedHandle, this, &AKingdomsCharacter::TrySetCurrentManaAgain, 1.f, false);
	}
}

void AKingdomsCharacter::TrySetCurrentManaAgain()
{
	if (StatusWidgetRef)
	{
		StatusWidgetRef->SetCurrentMana(GetCurrentMana() / MaxMana);
	}
}

#pragma endregion
