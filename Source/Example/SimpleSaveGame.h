// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SimpleSaveGame.generated.h"

USTRUCT()
struct FCharacterStats {
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, Category = "Player stats")
	FString equippedWeaponName;

	UPROPERTY(VisibleAnywhere, Category = "Player stats")
	FString levelName;

	UPROPERTY(VisibleAnywhere, Category = "Player stats")
	float health;

	UPROPERTY(VisibleAnywhere,  Category = "Player stats")
	float maxHealth;

	UPROPERTY(VisibleAnywhere, Category = "Player stats")
	float stamina;

	UPROPERTY(VisibleAnywhere, Category = "Player stats")
	float maxStamina;

	UPROPERTY(VisibleAnywhere, Category = "Player stats")
	int32 coins;

	UPROPERTY(VisibleAnywhere, Category = "Player stats")
	FVector location;

	UPROPERTY(VisibleAnywhere, Category = "Player stats")
	FRotator rotation;
};

/**
 * 
 */
UCLASS()
class EXAMPLE_API USimpleSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString playerName;
	
	UPROPERTY(VisibleAnywhere, Category = Basic)
	uint32 playerIndex;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FCharacterStats characterStats;
public:
	USimpleSaveGame();
};
