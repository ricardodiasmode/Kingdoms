// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KingdomsPlayerController.generated.h"

UCLASS()
class AKingdomsPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AKingdomsPlayerController();

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	// Begin PlayerController interface
	//virtual void PlayerTick(float DeltaTime) override;
	//virtual void SetupInputComponent() override;
	// End PlayerController interface

};


