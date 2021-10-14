// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class EXAMPLE_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Spawning)
	class UBoxComponent* spawningBox_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawning)
	TSubclassOf<AActor> actorToSpawn1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawning)
	TSubclassOf<AActor> actorToSpawn2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawning)
	TSubclassOf<AActor> actorToSpawn3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawning)
	TSubclassOf<AActor> actorToSpawn4;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	TSubclassOf<AActor> getRandActorToSpawn() const;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = Spawning)
	FVector getSpawnPoint() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Spawning")
	void spawnRandActor(const FVector& spawnLocation);
};
