// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Networking.h"
#include "Components/WidgetComponent.h"
#include "StatusBar.h"
#include "KingdomsCharacter.h"
#include "ItemPropertiesClass.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseEnemy.generated.h"

UCLASS()
class KINGDOMS_API ABaseEnemy : public ACharacter
{
	GENERATED_BODY()
	//// Variables ////
private:
	// Variable to hold pickable blueprint class
	TSubclassOf<class APickableItem> PickableBlueprintSpawn;

	// This bool will be readed only locally
	bool AmISelected;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentLife)
	float CurrentLife;
	UPROPERTY(ReplicatedUsing = OnRep_CurrentMana)
	float CurrentMana;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Widget, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* StatusWidget;

	// Time between quads(1.f = 1 quad per second)
	float MoveSpeed = 1.f;
	FTimerHandle MoveTimer;
	AKingdomsCharacter* CharacterToChase;
	// Location above of the quad to move
	FVector QuadToMove;

protected:
	UPROPERTY(EditAnywhere)
	TArray<UMaterialInstance*> SelectedMaterials;
	UPROPERTY(EditAnywhere)
	TArray<UMaterialInstance*> DefaultMaterials;

	UPROPERTY(EditAnywhere)
	float MaxLife = 100.f;
	UPROPERTY(EditAnywhere)
	float MaxMana = 100.f;

	UPROPERTY(EditAnywhere)
	float BaseDamage = 10.f;

	UPROPERTY(EditAnywhere)
	int ExperienceValue = 1;

	UPROPERTY(BlueprintReadWrite)
	FVector InitialSpawnLocation;

	UPROPERTY(EditAnywhere)
	TArray<FS_ItemDrop> ItemsToDrop;

	UStatusBar* StatusWidgetRef;

//// Methods ////
private:
	UFUNCTION()
	void OnRep_CurrentLife();
	void TrySetCurrentLifeAgain();
	UFUNCTION()
	void OnRep_CurrentMana();
	void TrySetCurrentManaAgain();

	void CheckShouldDie(AKingdomsCharacter* AgressiveCharacter);

	UFUNCTION(Server, reliable)
	void Server_DestroyMe(AKingdomsCharacter* AgressiveCharacter);

	// Recursive function that find from character to this enemy a avaliable path and return the first quad to this path
	FVector FindWayToCharacterOrQuad(FVector LastQuadLocation, bool TargetIsCharacter);
	FVector MoveDownUp(FVector LastQuadLocation, FVector FrontVector, bool TargetIsCharacter);
	FVector MoveLeftRight(FVector LastQuadLocation, FVector FrontVector, bool TargetIsCharacter);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this character's properties
	ABaseEnemy();

	// Called when close to character
	UFUNCTION(Server, Reliable, BlueprintCallable)
	virtual void Server_HitCharacter();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/* Called when clicked by player.
	*	return if it is selected.
	*/
	bool OnBeingClicked();

	/* Function that change material to default or selected 
	* @param IsSelected: whether or not will change to selected material
	*/
	void ChangeMaterials(bool IsSelected);

	// Called by server when character hits the enemy
	void LoseLife(const float Damage, AKingdomsCharacter* AgressiveCharacter);

	/* Function called by BTT to start to chase a player
	*/
	UFUNCTION(BlueprintCallable)
	void SetTimerToChasePlayer(AKingdomsCharacter* CharacterRef);

	/* Function called by BTT to move to a close random quad
	*/
	UFUNCTION(BlueprintCallable)
	void MoveToRandomQuad();

	void MoveToCharacterDirection();

	/* Function called by AIController to back to spawn after chase player
	*/
	UFUNCTION(BlueprintCallable)
	void GoBackToSpawn();

	void MoveToSpawnDirection();

	void SetCurrentLife(const float LifeToSet) { CurrentLife = (LifeToSet < 0) ? 0 : LifeToSet; OnRep_CurrentLife(); }
	FORCEINLINE float GetCurrentLife() { return CurrentLife; }
	FORCEINLINE float GetCurrentMana() { return CurrentMana; }
	
	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsMoving() const { return GetWorldTimerManager().IsTimerActive(MoveTimer); }
};
