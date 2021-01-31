// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Networking.h"
#include "Components/WidgetComponent.h"
#include "StatusBar.h"
#include "KingdomsCharacter.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseEnemy.generated.h"

UCLASS()
class KINGDOMS_API ABaseEnemy : public ACharacter
{
	GENERATED_BODY()
	//// Variables ////
private:
	// This bool will be readed only locally
	bool AmISelected;
	
	float CurrentLife;
	float CurrentMana;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Widget, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* StatusWidget;

	// Time between quads(1.f = 1 quad per second)
	float MoveSpeed = 1.f
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

	UStatusBar* StatusWidgetRef;

//// Methods ////
private:
	void CheckShouldDie();

	UFUNCTION(Server, reliable)
	void Server_DestroyMe();

	// Recursive function that find from character to this enemy a avaliable path and return the first quad to this path
	FVector FindWayToCharacter(FVector LastQuadLocation);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this character's properties
	ABaseEnemy();
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

	void LoseLife(const float Damage);

	/* Function called by BTT to start to chase a player
	*/
	UFUNCTION(BlueprintCallable)
	void SetTimerToChasePlayer(AKingdomsCharacter* CharacterRef);

	/* Function called by BTT to move to a close random quad
	*/
	UFUNCTION(BlueprintCallable)
	void MoveToRandomQuad();

	void MoveToQuad(FVector QuadToMove);

	/* Function called by AIController to back to spawn after chase player
	*/
	UFUNCTION(BlueprintCallable)
	void GoBackToSpawn();

	void SetCurrentLife(const float LifeToSet) { CurrentLife = (LifeToSet < 0) ? 0 : LifeToSet; }
	FORCEINLINE float GetCurrentLife() { return CurrentLife; }
};
