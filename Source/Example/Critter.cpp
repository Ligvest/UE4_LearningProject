// Fill out your copyright notice in the Description page of Project Settings.


#include "Critter.h"

// Sets default values
ACritter::ACritter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	currentVelocity = FVector(0.f);
	maxSpeed = 100.f;


	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("rootComponent"));
	meshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("meshComponent"));
	meshComponent->SetupAttachment(GetRootComponent());

	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("camera"));
	camera->SetupAttachment(GetRootComponent());
	camera->SetRelativeLocation(FVector(-300.f, 0.f, 300.f));
	camera->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));

//	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ACritter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACritter::moveForward(float value)
{
	currentVelocity.X = FMath::Clamp(value, -1.f, 1.f) * maxSpeed;
}

void ACritter::moveRight(float value)
{
	currentVelocity.Y = FMath::Clamp(value, -1.f, 1.f) * maxSpeed;
}

// Called every frame
void ACritter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AddActorLocalOffset(currentVelocity * DeltaTime);
	//FVector newLocation = GetActorLocation() + (currentVelocity * DeltaTime);
	//SetActorLocation(newLocation);
}

// Called to bind functionality to input
void ACritter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &ACritter::moveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACritter::moveRight);
}

