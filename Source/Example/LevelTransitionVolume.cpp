// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTransitionVolume.h"
#include "Components/BoxComponent.h"
#include "Components/BillboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MainCharacter.h"

// Sets default values
ALevelTransitionVolume::ALevelTransitionVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	transitionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TransitionVolume"));
	RootComponent = transitionVolume;

	billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	billboard->SetupAttachment(GetRootComponent());

	// Initialize a new level name "levelNameToSwitchOn" with current level name
	// to not switch to a new level if new level name is not set
	UWorld* world = GetWorld();
	if (world) {
		FString currentLevelName = UGameplayStatics::GetCurrentLevelName(world);
		FName currentLevelFName = *currentLevelName;
		levelNameToSwitchOn = currentLevelFName;
	}
}

// Called when the game starts or when spawned
void ALevelTransitionVolume::BeginPlay()
{
	Super::BeginPlay();

	transitionVolume->OnComponentBeginOverlap.AddDynamic(this, &ALevelTransitionVolume::onBeginOverlap);
}

void ALevelTransitionVolume::onBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMainCharacter* mainCharacter = Cast<AMainCharacter>(OtherActor);
	if (mainCharacter) {
		mainCharacter->saveGame(ESaveLoadType::ESL_LevelTransition);
		mainCharacter->switchLevel(levelNameToSwitchOn);
		// Game is loading in BeginPlay() of mainCharcter
	}
}

// Called every frame
void ALevelTransitionVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

