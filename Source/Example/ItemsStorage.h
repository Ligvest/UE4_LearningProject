// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemsStorage.generated.h"

UCLASS()
class EXAMPLE_API AItemsStorage : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Save data")
	TMap<FString, TSubclassOf<class AWeapon>> weaponsMap;

public:	
	// Sets default values for this actor's properties
	AItemsStorage();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
