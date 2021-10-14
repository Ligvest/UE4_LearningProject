// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"
#include "MainCharacter.h"

void UMainAnimInstance::NativeInitializeAnimation()
{
	if (pawn_ == nullptr) {
		pawn_ = TryGetPawnOwner();
		if (pawn_) {
			mainCharacter_ = Cast<AMainCharacter>(pawn_);
		}
	}
}

void UMainAnimInstance::updateAnimationProperties()
{
//	NativeInitializeAnimation();

	if (pawn_) {
		FVector pawnVelocity = pawn_->GetVelocity();
		FVector lateralSpeed(pawnVelocity.X, pawnVelocity.Y, 0.f);
		movementSpeed_ = lateralSpeed.Size();

		isInAir_ = pawn_->GetMovementComponent()->IsFalling();
		if (!mainCharacter_) {
			mainCharacter_ = Cast<AMainCharacter>(pawn_);
		}
	}
}
