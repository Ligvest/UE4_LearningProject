// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyMovementStatus : uint8 {
	EMS_Idle UMETA(DisplayName = "Idle"),
	EMS_Moves UMETA(DisplayName = "Moves"),
	EMS_Attacks UMETA(DisplayName = "Attacks"),
	EMS_Dead UMETA(DisplayName = "Dead"),

	EMS_MAX UMETA(DisplayName = "DefaultMax")
};


UCLASS()
class EXAMPLE_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties	
	AEnemy();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	
	UFUNCTION()
	virtual void agroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void agroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void combatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void combatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FORCEINLINE void setMovementStatus(const EEnemyMovementStatus newStatus) { movementStatus_ = newStatus; }

	// Called from EnemyAnim blueprint on "AttackEnd" notification
	UFUNCTION(BlueprintCallable)
	void attackEnd();

	UFUNCTION()
	void attack();

	UFUNCTION(BlueprintPure)
	EEnemyMovementStatus getMovementStatus() { return movementStatus_; }

	UFUNCTION()
	virtual void attackCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void attackCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void activateAttackCollision();

	UFUNCTION(BlueprintCallable)
	void deactivateAttackCollision();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void playAttackSound();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable, Category = "Status")
	void onDeathAnimationEnd();

	UFUNCTION(BlueprintCallable, Category = "Status")
	bool isAlive();

	UFUNCTION(BlueprintCallable, Category = "Status")
	float getNormalizedHealth() const;

	void showHealth();

	void setController(class AAIController* newController) { controller_ = newController; }

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float removeCorpseDelay_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* agroSphere_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	USphereComponent* combatSphere_;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	class UParticleSystem* takeDamageParticles_;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	class USoundCue* takeDamageSound_;

	float health_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float maxHealth_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float damage_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float attackSpeed_;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	class USoundCue* attackSound_;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	class UBoxComponent* attackCollision_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsAttacking_;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* combatMontage_;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float minAttackDelay_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float maxAttackDelay_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> damageTypeClass_;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void moveToTarget(class AMainCharacter* mainCharacter);

	void stopChasing();

	void die();

	void removeCorpse();


	void hideHealth();

	void clearCombatTargetLinks();


protected:
	EEnemyMovementStatus movementStatus_;
	AAIController* controller_;
	bool bIsMainCharacterInCombatSphere_;
	bool bIsMainCharacterInAgroSphere_;
	class AMainCharacter* combatTarget_;
	FTimerHandle attackDelayTimerHandle_;
	FTimerHandle removeCorpseTimerHandle_;



};
