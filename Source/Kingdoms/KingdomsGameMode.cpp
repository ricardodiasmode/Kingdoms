// Copyright Epic Games, Inc. All Rights Reserved.

#include "KingdomsGameMode.h"
#include "KingdomsPlayerController.h"
#include "KingdomsCharacter.h"
#include "UObject/ConstructorHelpers.h"

AKingdomsGameMode::AKingdomsGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AKingdomsPlayerController::StaticClass();
}