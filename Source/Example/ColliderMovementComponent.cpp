// Fill out your copyright notice in the Description page of Project Settings.


#include "ColliderMovementComponent.h"


void UColliderMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime)) {
		return;
	}

	FVector desiredMovementThisFrame = ConsumeInputVector();//.GetClampedToMaxSize(1.f);

	if (!desiredMovementThisFrame.IsNearlyZero()) {
		FHitResult hit;
		SafeMoveUpdatedComponent(desiredMovementThisFrame, UpdatedComponent->GetComponentRotation(), true, hit);

		if (hit.IsValidBlockingHit()) {
			// SlideAlongSurface let us to keep moving when
			// we are moving against obstacles like wall. 
			// Instead of stucking we just slowly slide
			// hit.Time - from 0 to 1. 1 is a delta time?
			SlideAlongSurface(desiredMovementThisFrame, 1.f - hit.Time, hit.Normal, hit);
		}
	}
}
