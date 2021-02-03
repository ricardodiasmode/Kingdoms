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

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a decal in the world to show the cursor's location
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
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
						Server_OnSelectEnemy(InimigoAtingido);
					else
						OnDeselectEnemy();
				}
			}
		}
	}
}

void AKingdomsCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		if (Value > 0)
		{
			LastMoveKey = "W";
			Server_SetRotationOnMoving("W");
			if (!HasAuthority())
				Multicast_SetRotationOnMoving(0.f);
		}
		else
		{
			LastMoveKey = "S";
			Server_SetRotationOnMoving("S");
			if (!HasAuthority())
				Multicast_SetRotationOnMoving(180.f);
		}
		float AdditionalMove = 100 * Value;
		FVector LocalLocationToMove(GetActorLocation().X + AdditionalMove, GetActorLocation().Y, GetActorLocation().Z);
		Server_MoveWithMoveSpeed(LocalLocationToMove);
	}
}

void AKingdomsCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		if (Value > 0)
		{
			LastMoveKey = "D";
			Server_SetRotationOnMoving("D");
			if(!HasAuthority())
				Multicast_SetRotationOnMoving(90.f);
		}
		else
		{
			LastMoveKey = "A";
			Server_SetRotationOnMoving("A");
			if (!HasAuthority())
				Multicast_SetRotationOnMoving(270.f);

		}
		float AdditionalMove = 100 * Value;
		FVector LocalLocationToMove(GetActorLocation().X, GetActorLocation().Y + AdditionalMove, GetActorLocation().Z);
		Server_MoveWithMoveSpeed(LocalLocationToMove);
	}
}

void AKingdomsCharacter::Server_SetRotationOnMoving_Implementation(const FString& KeyRef)
{
	if (KeyRef == "W")
		Multicast_SetRotationOnMoving(0.f);
	if (KeyRef == "A")
		Multicast_SetRotationOnMoving(270.f);
	if (KeyRef == "S")
		Multicast_SetRotationOnMoving(180.f);
	if (KeyRef == "D")
		Multicast_SetRotationOnMoving(90.f);
}

void AKingdomsCharacter::Server_MoveWithMoveSpeed_Implementation(const FVector Location)
{
	if (CanMoveWithMoveSpeed)
	{
		CanMoveWithMoveSpeed = false;
		LocationToMove = Location;
		IsMoving = true;
		if (!(GetWorldTimerManager().IsTimerActive(MoveTimer)))
			GetWorldTimerManager().SetTimer(MoveTimer, this, &AKingdomsCharacter::MoveToDesiredLocation, 0.001f, true, 0.0f);
	}
}

void AKingdomsCharacter::MoveToDesiredLocation()
{
	FHitResult OutSweepHitResult;
	// Using ease to make a smooth movement
	FVector EaseResult(UKismetMathLibrary::Ease(GetActorLocation().X, LocationToMove.X, CurrentMoveSpeed / 100.f, EEasingFunc::Linear),
		UKismetMathLibrary::Ease(GetActorLocation().Y, LocationToMove.Y, CurrentMoveSpeed / 100.f, EEasingFunc::Linear),
		UKismetMathLibrary::Ease(GetActorLocation().Z, LocationToMove.Z, CurrentMoveSpeed / 100.f, EEasingFunc::Linear));
	SetActorLocation(EaseResult, false, &OutSweepHitResult, ETeleportType::None);
	// Check if the location is close or is already in the goal
	if (EaseResult.Equals(LocationToMove, 5.f))
	{
		SetActorLocation(LocationToMove, false, &OutSweepHitResult, ETeleportType::None);
		if (!(GetWorldTimerManager().IsTimerActive(MoveTimer)))
			GetWorldTimerManager().ClearTimer(MoveTimer);
		CanMoveWithMoveSpeed = true;
		if(LastMoveKey == "A")
			SetActorRotation(FRotator(0.f, 270.f, 0.f), ETeleportType::None);
		else if(LastMoveKey == "D")
			SetActorRotation(FRotator(0.f, 90.f, 0.f), ETeleportType::None);
		else if(LastMoveKey == "W")
			SetActorRotation(FRotator(0.f, 0.f, 0.f), ETeleportType::None);
		else
			SetActorRotation(FRotator(0.f, 180.f, 0.f), ETeleportType::None);
		AlphaRotation = 0.f;
		IsMoving = false;
	}
}

void AKingdomsCharacter::Multicast_SetRotationOnMoving_Implementation(float Direction)
{
	FRotator TestRotation;
	TestRotation.Yaw = Direction;
	// Testing if the rotation is next to goal to not run the function for nothing
	if (GetActorRotation().Equals(TestRotation, 5.f))
	{
		SetActorRotation(FRotator(0.f, Direction, 0.f), ETeleportType::None);
		return;
	}
	else
	{
		if (GetActorRotation().Equals(TestRotation, 5.f))
		{
			SetActorRotation(FRotator(0.f, Direction, 0.f), ETeleportType::None);
			return;
		}
	}
	// Using ease to do a smooth rotation movement
	AlphaRotation += 0.175f;
	AlphaRotation = FMath::Clamp(AlphaRotation, 0.0f, 1.f);
	// Converting Yaw to make sure that it will me positive values to not flick the character
	float EaseYaw;
	float EaseResult;
	if (UKismetMathLibrary::Abs(GetActorRotation().Yaw - Direction) > UKismetMathLibrary::Abs(GetActorRotation().Yaw+360 - Direction))
	{
		EaseYaw = GetActorRotation().Yaw + 360;
		EaseResult = UKismetMathLibrary::Ease(EaseYaw, Direction, AlphaRotation, EEasingFunc::Linear);
	}
	else
	{
		EaseYaw = GetActorRotation().Yaw;
		EaseResult = UKismetMathLibrary::Ease(EaseYaw, Direction, AlphaRotation, EEasingFunc::Linear);
	}
	FRotator CurrentRotation;
	CurrentRotation.Yaw = EaseResult;
	SetActorRotation(CurrentRotation, ETeleportType::None);
}

void AKingdomsCharacter::SetAttackRange()
{
	if (FantasyClass == CharacterClass::Knight)
		AttackRange = 1.f;
}

void AKingdomsCharacter::BeginPlay()
{
	Super::BeginPlay();
	SetAttackRange();
}

void AKingdomsCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

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
				World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params);
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

void AKingdomsCharacter::OnDeselectEnemy()
{
	Server_OnDeselectEnemy();
}

bool AKingdomsCharacter::Server_OnSelectEnemy_Validate(class ACharacter* EnemyToHit)
{
	if (EnemyToHit != NULL)
		return true;
	return false;
}

void AKingdomsCharacter::Server_OnSelectEnemy_Implementation(class ACharacter* EnemyToHit)
{
	if (EnemyToHit != NULL)
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
				NPCToHit->LoseLife(GetCurrentBaseDamage());
		}
	}
}
