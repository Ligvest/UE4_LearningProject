// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/PawnMovementComponent.h"
#include "MainAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class EXAMPLE_API UMainAnimInstance : public UAnimInstance
{
	GENERATED_BODY()


public:
		virtual void NativeInitializeAnimation() override;

		UFUNCTION(BlueprintCallable, Category = AnimationProperties)
		void updateAnimationProperties();

		UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Movement)
		float movementSpeed_;

		UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Movement)
		bool isInAir_;

		UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Movement)
		APawn* pawn_;

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
		class AMainCharacter* mainCharacter_;
};
