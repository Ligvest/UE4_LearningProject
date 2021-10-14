// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "Enemy.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	if (!pawn_) {
		pawn_ = TryGetPawnOwner();
		if (pawn_) {
			enemy_ = Cast<AEnemy>(pawn_);
		}
	}
}

void UEnemyAnimInstance::updateAnimationProperties()
{
	if (!pawn_) {
		pawn_ = TryGetPawnOwner();
		if (pawn_) {
			enemy_ = Cast<AEnemy>(pawn_);
		}
	}

	if (pawn_) {
		FVector pawnVelocity = pawn_->GetVelocity();
		FVector lateralSpeed(pawnVelocity.X, pawnVelocity.Y, 0.f);
		movementSpeed_ = lateralSpeed.Size();
	}
}
