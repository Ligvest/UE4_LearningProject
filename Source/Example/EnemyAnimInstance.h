// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class EXAMPLE_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Movement)
	float movementSpeed_;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Movement)
	APawn* pawn_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	class AEnemy* enemy_ = nullptr;


public:
	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable, Category = AnimationProperties)
	void updateAnimationProperties();
};
