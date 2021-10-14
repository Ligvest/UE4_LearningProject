// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelTransitionVolume.generated.h"

UCLASS()
class EXAMPLE_API ALevelTransitionVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelTransitionVolume();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transition")
	class UBoxComponent* transitionVolume;

	class UBillboardComponent* billboard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
	FName levelNameToSwitchOn;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void onBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
