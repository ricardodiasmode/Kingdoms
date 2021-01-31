// Copyright Epic Games, Inc. All Rights Reserved.

#include "KingdomsPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "KingdomsCharacter.h"
#include "Engine/World.h"

AKingdomsPlayerController::AKingdomsPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}
