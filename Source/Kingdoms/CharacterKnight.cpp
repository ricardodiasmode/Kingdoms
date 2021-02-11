// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterKnight.h"

ACharacterKnight::ACharacterKnight()
{
	CurrentBaseDamage = 10.f;
	FantasyClass = CharacterClass::Knight;
}

void ACharacterKnight::SetAttackRange()
{
	AttackRange = 1.f;
}