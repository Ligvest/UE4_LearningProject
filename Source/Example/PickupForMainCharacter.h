// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "PickupForMainCharacter.generated.h"

/**
 * 
 */
UCLASS()
class EXAMPLE_API APickupForMainCharacter : public AItem
{
	GENERATED_BODY()

public:
	APickupForMainCharacter();


	virtual void onBeginOverlap(UPrimitiveComponent* OverlappedComponent,
				AActor* OtherActor,
				UPrimitiveComponent* OtherComp,
				int32 OtherBodyIndex,
				bool bFromSweep,
				const FHitResult& SweepResult) override;

	virtual void onEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;


	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void onPickupActionBP(class AMainCharacter* character);
};
