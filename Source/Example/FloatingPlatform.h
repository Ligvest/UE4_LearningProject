// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingPlatform.generated.h"

UCLASS()
class EXAMPLE_API AFloatingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingPlatform();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Platform)
	UStaticMeshComponent* mesh_;

	UPROPERTY(EditAnywhere, Category = Platform)
	FVector startPoint_;

	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"), Category = Platform)
	FVector endPoint_;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Platform)
	float interpSpeed_;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Platform)
	float delayBetweenTogglingDirections_;

	FTimerHandle changeDirectionTimerHandle;

	bool bChangingDirection;

protected:
	UPROPERTY(VisibleDefaultsOnly, Category = "Platform")
	class UBoxComponent* standBoxTrigger_;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void updateLocation(float deltaTime); 
	void swapVectors(FVector& first, FVector& second);
	void changeMoveDirection();

	UFUNCTION(Category = "Platform")
	void standBoxTriggerOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
