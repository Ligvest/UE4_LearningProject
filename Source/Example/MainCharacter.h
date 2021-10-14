// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MainCharacter.generated.h"


UENUM(BlueprintType)
enum class ESaveLoadType : uint8 {
	ESL_User UMETA(DisplayName = "User"),
	ESL_LevelTransition UMETA(DisplayName = "LevelTransition"),

	ESL_MAX UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EMovementStatus : uint8 {
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Dead UMETA(DisplayName = "Dead"),

	EMS_MAX UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8 {
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_Low UMETA(DisplayName = "Low"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),

	ESS_MAX UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class EXAMPLE_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()


public:
	// Sets default values for this character's properties
	AMainCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



	void setOverlappedItem(class AItem* newOverlappedItem);

	/**
	* @param rate This is a normalized rate ( i.e. 1.0 is 100% of baseRate)
	*/
	void turnAtRate(float rate);

	/**
	* @param rate This is a normalized rate ( i.e. 1.0 is 100% of baseRate)
	*/
	void lookUpAtRate(float rate);

	FORCEINLINE USpringArmComponent* getCameraBoom() const { return cameraBoom; }
	FORCEINLINE UCameraComponent* getFollowCamera() const { return followCamera; }

	void attack();

	void attackButtonPressed();
	void attackButtonReleased();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void attackEnd();


	UFUNCTION(BlueprintPure, Category = "Weapon")
	bool isArmed();

	UFUNCTION(BlueprintPure, Category = "Player stats")
	float getNormalizedHealth() const;

	UFUNCTION(BlueprintPure, Category = "Player stats")
	float getNormalizedStamina() const;

	UFUNCTION(BlueprintPure, Category = "Player stats")
	int32 getCoins() const;


	UFUNCTION(BlueprintCallable, Category = "Player stats")
	void increaseCoins(int32 numberOfCoins);

	UFUNCTION(BlueprintCallable, Category = "Player stats")
	void increaseHealth(float healthAmount);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void setEquippedWeapon(AWeapon* newEquippedWeapon);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	AWeapon* getEquippedWeapon();

	void die();

	void setCombatTarget(class AEnemy* newCombatTarget);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	AEnemy* getCombatTarget();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void onDeathAnimationEnd();

	void updateCombatTarget(); // Assigns the closest enemy to combatTarget_ variable

	AEnemy* findClosestEnemy();

	void switchLevel(FName newLevelName);

	UFUNCTION(BlueprintCallable, Category = "Save game")
	void saveGame(ESaveLoadType eSaveType);

	UFUNCTION(BlueprintCallable, Category = "Save game")
	void loadGame(ESaveLoadType eLoadType);

	void bindActionsAndAxes();

	void clearBoundActionsAndAxes();

public:
	UPROPERTY(EditDefaultsOnly, Category = "Save game")
	TSubclassOf<class AItemsStorage> weaponTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<AEnemy> enemyFilter;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "Combat")
	TArray<AActor*> overlappingEnemies;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category  = "Animations")
	class UAnimMontage* combatMontage_;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* cameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* followCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = Item)
	class AItem* overlappedItem_;

	/** turn rate  and look up rate to control sensitivity of camera when using buttons and not a mouse */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float baseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float baseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player stats")
	float health_;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player stats")
	float maxHealth_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player stats")
	float stamina_;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player stats")
	float maxStamina_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player stats")
	int32 coins_;

	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	bool bIsSprinting_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	int32 normalMovementSpeed_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	int32 sprintMovementSpeed_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	EStaminaStatus staminaStatus_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	EMovementStatus movementStatus_;

	// The level of stamina when it will be considered as low
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float staminaLowLevel_;

	// Per second
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float staminaDrainRate_;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	class UParticleSystem* takeDamageParticles_;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	class USoundCue* takeDamageSound_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsAttacking_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float interpolateRotationSpeed_;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bNewLevel;

protected:
	// Protected functions
	void calcSprintMovementSpeed();
	void updateStamina(float deltaStamina);
	void setMovementStatus(EMovementStatus newMovementStatus);
	void setStaminaStatus(EStaminaStatus newStaminaStatus);
	void updateStaminaAndMovementStatuses(float deltaTime);
	FRotator getLookAtEnemyYawRotator();
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	void spawnSavedWeapon(class USimpleSaveGame* loadGameObject);

	// Bindings
	void moveForward(float value);
	void moveRight(float value);
	bool moves();
	void sprintButtonPressed();
	void sprintButtonReleased();
	void interactButtonPressed();
	void interactButtonReleased();
	void pauseButtonPressed();
	void pauseButtonReleased();
	void setLoadingByUser(bool loadingByUser);
	bool getLoadingByUser();
	bool firstGameStart();
	void setFirstGameStart(bool isFirstGameStart);


protected:
	// Protected members
	class AWeapon* equippedWeapon_; // current equipped weapon

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class AEnemy* combatTarget_; // a Enemy which combatSphere is overlapped by the character

	bool bIsInteractButtonPressed_;
	bool bIsAttackButtonPressed_;
	class UInputComponent* playerInputComponentReference;
	bool bIsMovingForward_;
	bool bIsMovingRight_;
};
