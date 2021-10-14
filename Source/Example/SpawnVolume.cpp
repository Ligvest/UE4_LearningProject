// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Enemy.h"
#include "AIController.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	spawningBox_ = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));

}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{

	Super::BeginPlay();
	
}

TSubclassOf<AActor> ASpawnVolume::getRandActorToSpawn() const
{
	TSubclassOf<AActor> actorToSpawn;
	if (actorToSpawn1 && actorToSpawn2 && actorToSpawn3 && actorToSpawn4) {
		TArray<TSubclassOf<AActor>> poolOfActors{ actorToSpawn1, actorToSpawn2,actorToSpawn3, actorToSpawn4 };

		if (poolOfActors.Num() > 0) {
			int32 actorNumber = FMath::RandRange(0, poolOfActors.Num() - 1);
			actorToSpawn = poolOfActors[actorNumber];
		}
	}
	return actorToSpawn;
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ASpawnVolume::getSpawnPoint() const
{
	FVector origin = spawningBox_->GetComponentLocation();
	FVector extent = spawningBox_->GetScaledBoxExtent();

	FVector randPoint = UKismetMathLibrary::RandomPointInBoundingBox(origin, extent);

	return randPoint;
}

void ASpawnVolume::spawnRandActor_Implementation(const FVector& spawnLocation)
{
	TSubclassOf<AActor> actorToSpawn = getRandActorToSpawn();


	if (actorToSpawn) {
		UWorld* world = GetWorld();

		if (world) {
			FActorSpawnParameters spawnParameters;
			AActor* spawnedActor = world->SpawnActor<AActor>(actorToSpawn, spawnLocation, FRotator(0.f), spawnParameters);

			//The default behavior is that SPAWNED Pawns don't get their AIControllers spawned. 
			// This is why we create a new controller by calling SpawnDefaultController
			AEnemy* spawnedEnemy = Cast<AEnemy>(spawnedActor);
			if (spawnedEnemy) // if spawned actor is an enemy
			{
				spawnedEnemy->SpawnDefaultController();
				AAIController* controller = Cast<AAIController>(spawnedEnemy->GetController());
				if (controller) {
					spawnedEnemy->setController(controller);
				}

			}
			
		}
	}
}
