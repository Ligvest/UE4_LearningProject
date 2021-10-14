// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Critter.generated.h"

UCLASS()
class EXAMPLE_API ACritter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACritter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void moveForward(float value);
	void moveRight(float value);



	// variables
	FVector currentVelocity;

	UPROPERTY(EditAnywhere, category = "Movement")
	float maxSpeed;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, category = "Mesh")
	USkeletalMeshComponent* meshComponent;

	UPROPERTY(EditAnywhere, category = "Mesh")
	UCameraComponent* camera;
};
