// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

//#include "Networking.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/InputSettings.h"
#include "FunctionLibrary.h"
#include "Components/WidgetComponent.h"
#include "StatusBar.h"
#include "ExperienceBar.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "KingdomsCharacter.generated.h"

UCLASS(Blueprintable)
class AKingdomsCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	//// Components ////
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Widget, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* StatusWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Widget, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* ExperienceWidget;

	//// Variables ////
private:
	// Character base move speed. The greater? it is, the faster the character moves
	float CurrentMoveSpeed = 1.f;
	// Character base attack speed. The lower it is, the faster the character hits
	float CurrentAttackSpeed = 1.f;
	// Character base attack range. Based in squares (1 means 100uu)
	float AttackRange = 1.f;
	// Character current experience to upgrade the hero stats
	UPROPERTY(Replicated)
	int CurrentExperience = 0;
	UPROPERTY(Replicated)
	int RequiredExperienceToUp = 1 + pow(CurrentLevel,2);
	// Character current level
	UPROPERTY(Replicated)
	int CurrentLevel = 1;

	UPROPERTY(EditAnywhere)
	float MaxLife = 100.f;
	UPROPERTY(EditAnywhere)
	float MaxMana = 100.f;

	float CurrentLife;
	float CurrentMana;

	FTimerHandle HitEnemyTimer;
	// Function that hits the enemy called by server
	void HitEnemy();
	FString LastMoveKey;
	bool CanMoveWithMoveSpeed = true;
	FTimerHandle MoveTimer;
	FVector LocationToMove;
	void MoveToDesiredLocation();
	float AlphaRotation = 0;

protected:
	// Character fantasy class
	CharacterClass FantasyClass;
	// Character base damage.
	float CurrentBaseDamage = 0.f;
	// Variable readed and setted by server to be used in the HitEnemy method using timer.
	class ACharacter* CurrentlyHittedEnemy;

	UStatusBar* StatusWidgetRef;
	UExperienceBar* ExperienceWidgetRef;
public:
	UPROPERTY(BlueprintReadOnly, Replicated)
	bool IsMoving;

	//// Methods ////
private:
	void SetAttackRange();

	UFUNCTION(Server, reliable)
	void Server_SetRotationOnMoving(const FString& KeyRef);

	UFUNCTION(Server, unreliable)
	void Server_MoveWithMoveSpeed(const FVector Location);

	UFUNCTION(NetMulticast, unreliable)
	void Multicast_SetRotationOnMoving(float Direction);

	UFUNCTION(Server, reliable, WithValidation)
	void Server_OnSelectEnemy(class ACharacter* EnemyToHit);
	UFUNCTION(BlueprintCallable)
	void OnDeselectEnemy();
	UFUNCTION(Server, reliable)
	void Server_OnDeselectEnemy();

	void OnClick();

public:

	void RecieveDamage(float DamageToRecieve);
	void AddExperience(int ExperienceToAdd);

	FORCEINLINE float GetCurrentMana() const { return CurrentMana; }
	FORCEINLINE float GetCurrentLife() const { return CurrentLife; }
	FORCEINLINE float GetCurrentMoveSpeed() const { return CurrentMoveSpeed; }
	FORCEINLINE float GetCurrentAttackSpeed() const { return CurrentAttackSpeed; }
	FORCEINLINE float GetCurrentBaseDamage() const { return CurrentBaseDamage; }
	
/////////////// Default things ///////////////
public:
	AKingdomsCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

	
protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;
};

