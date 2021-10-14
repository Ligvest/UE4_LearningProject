// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SphereComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "ColliderMovementComponent.h"
#include "Collider.generated.h"

UCLASS()
class EXAMPLE_API ACollider : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACollider();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void moveForward(float value);
	void moveRight(float value);

	void yawCamera(float axisValue);
	void pitchCamera(float axisValue);

	FVector2D cameraInput;
	FRotator cameraRotator;

	virtual UPawnMovementComponent* GetMovementComponent() const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// A mesh
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	UStaticMeshComponent* meshComponent;

	// Collision sphere
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	USphereComponent* sphereComponent;

	// Camera
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	UCameraComponent* cameraComponent;

	// Spring arm
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	USpringArmComponent* springArm;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	UColliderMovementComponent* ourMovementComponent;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float speedMultiplier;

	FORCEINLINE UStaticMeshComponent* getMeshComponent() { return meshComponent; }
	FORCEINLINE void setMeshComponent(UStaticMeshComponent* mesh) { meshComponent = mesh; }

	FORCEINLINE USphereComponent* getSphereComponent() { return sphereComponent; }
	FORCEINLINE void setSphereComponent(USphereComponent* sphere) { sphereComponent = sphere; }

	FORCEINLINE UCameraComponent* getCameraComponent() { return cameraComponent; }
	FORCEINLINE void setCameraComponent(UCameraComponent* newCameraComponent) { cameraComponent = newCameraComponent; }

	FORCEINLINE	USpringArmComponent* getSpringArmComponent() { return springArm; }
	FORCEINLINE	void setSpringArmComponent(USpringArmComponent* newSpringArmComponent) { springArm = newSpringArmComponent; }

};
