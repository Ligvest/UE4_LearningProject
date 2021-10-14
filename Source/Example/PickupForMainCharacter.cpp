// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupForMainCharacter.h"
#include "MainCharacter.h"

APickupForMainCharacter::APickupForMainCharacter() {
}

void APickupForMainCharacter::onBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::onBeginOverlap(OverlappedComponent,  OtherActor, OtherComp, OtherBodyIndex,  bFromSweep, SweepResult);
	if (OtherActor) {
		AMainCharacter* character = Cast<AMainCharacter>(OtherActor);
		if (character) {
			onPickupActionBP(character);
			Destroy();
		}
	}
}

void APickupForMainCharacter::onEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::onEndOverlap(OverlappedComponent,  OtherActor,  OtherComp,  OtherBodyIndex);
}
