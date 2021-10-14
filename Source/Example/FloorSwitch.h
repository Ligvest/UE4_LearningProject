// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloorSwitch.generated.h"

UCLASS()
class EXAMPLE_API AFloorSwitch : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloorSwitch();

public:
	/** An area which will execute mechanism after character will step in it */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FloorSwitch)
	class UBoxComponent* triggerBox_;

	/** A plate on which character will step to trigger a door opening */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FloorSwitch)
	UStaticMeshComponent* floorSwitch_;

	/** A door which will be opened after floorSwitch is stepped on */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FloorSwitch)
	UStaticMeshComponent* door_;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = FloorSwitch)
	FVector initialFloorSwitchLocation_;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = FloorSwitch)
	FVector initialDoorLocation_;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = FloorSwitch)
	float switchTimeDelay;



	FTimerHandle switchTimerHandle;

private:

	bool bIsCharacterOnFloorSwitch;

	void switchTimerHasFired();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void onOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void onOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = FloorSwitch)
	void eRaiseDoor();

	UFUNCTION(BlueprintImplementableEvent, Category = FloorSwitch)
	void eLowerDoor();

	UFUNCTION(BlueprintImplementableEvent, Category = FloorSwitch)
	void eLowerFloorSwitch();

	UFUNCTION(BlueprintImplementableEvent, Category = FloorSwitch)
	void eRaiseFloorSwitch();

	UFUNCTION(BlueprintCallable, Category = FloorSwitch)
	void addHeightToDoorInitialLocation(float heightToAdd);

	UFUNCTION(BlueprintCallable, Category = FloorSwitch)
	void subtractHeightFromSwitchInitialLocation(float heightToSubtract);
};
