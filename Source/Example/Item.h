// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class EXAMPLE_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Collision")
	class USphereComponent* collisionVolume_;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Mesh")
	class UStaticMeshComponent* mesh_;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Particles")
	class UParticleSystemComponent* idleParticlesComponent_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Particles")
	class UParticleSystem* overlapParticles_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Particles")
	class USoundCue* overlapSound_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | ItemProperties")
	bool bShouldRotate_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | ItemProperties")
	float rotationRate_;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void onBeginOverlap(UPrimitiveComponent* OverlappedComponent, 
						AActor* OtherActor, 
						UPrimitiveComponent* OtherComp, 
						int32 OtherBodyIndex, 
						bool bFromSweep, 
						const FHitResult& SweepResult);

	UFUNCTION()
	virtual void onEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
