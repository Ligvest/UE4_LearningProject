// Fill out your copyright notice in the Description page of Project Settings.



#include "MainCharacter.h"
#include "MainPlayerController.h"
#include "Item.h"
#include "Weapon.h"
#include "Enemy.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/InputComponent.h"
#include "SimpleSaveGame.h"
#include "ItemsStorage.h"
#include "MyGameInstance.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	cameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("cameraBoom"));
	cameraBoom->SetupAttachment(GetRootComponent());
	cameraBoom->TargetArmLength = 600.f;
	cameraBoom->bUsePawnControlRotation = true; // Rotate camera based on controller

	followCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("followCamera"));
	followCamera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName); // SocketName is the top edge cameraBoom socket
	followCamera->bUsePawnControlRotation = false; // Rotate camera based on controller

	// set size for collision capsule
	GetCapsuleComponent()->SetCapsuleSize(34.f, 63.f);
	
	baseTurnRate = 65.f;
	baseLookUpRate = 65.f;

	// Whether rotate pawn when a camera rotates.
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// Aim the character to the movement direction with RotationRate rate
	UCharacterMovementComponent* characterMovement = GetCharacterMovement();
	characterMovement->bOrientRotationToMovement = true;
	characterMovement->RotationRate = FRotator(0.f, 500.f, 0.f);
	// The strength of jump
	characterMovement->JumpZVelocity = 650.f;
	// Level of cotrolling character in the air
	characterMovement->AirControl = 1.f;

	maxHealth_ = 100.f;
	health_ = maxHealth_;
	coins_ = 0;


	/**  movement and stamina */
	bIsSprinting_ = false;
	normalMovementSpeed_ = 650.f;
	calcSprintMovementSpeed(); // Initialize sprintMovementSpeed_
	movementStatus_ = EMovementStatus::EMS_Normal;

	maxStamina_ = 100.f;
	stamina_ = maxStamina_;
	staminaDrainRate_ = 20.f;
	staminaLowLevel_ = 30.f;
	staminaStatus_ = EStaminaStatus::ESS_Normal;


	bIsInteractButtonPressed_ = false;
	overlappedItem_ = nullptr;
	equippedWeapon_ = nullptr;

	combatMontage_ = nullptr;

	bIsAttacking_ = false;
	bIsAttackButtonPressed_ = false;

	interpolateRotationSpeed_ = 15.f;

	bIsMovingForward_ = false;
	bIsMovingRight_ = false;

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (firstGameStart()) {
		setFirstGameStart(false);
		return;
	}
	if (getLoadingByUser()) {
		loadGame(ESaveLoadType::ESL_User);
	}
	else {
		loadGame(ESaveLoadType::ESL_LevelTransition);
	}
}

void AMainCharacter::calcSprintMovementSpeed()
{
	sprintMovementSpeed_ = normalMovementSpeed_ * 1.5;
}

void AMainCharacter::updateStamina(float deltaStamina)
{
	if (bIsSprinting_ && moves()) {
		stamina_ -= deltaStamina;
		if (stamina_ < 0) {
			setStaminaStatus(EStaminaStatus::ESS_Exhausted);
			stamina_ = 0;
		}
	}
	else {
		stamina_ += deltaStamina;
		if (stamina_ > maxStamina_) {
			stamina_ = maxStamina_;
		}
	}
}

void AMainCharacter::setMovementStatus(EMovementStatus newMovementStatus)
{
	movementStatus_ = newMovementStatus;
	switch (movementStatus_) {
	case EMovementStatus::EMS_Normal:
		GetCharacterMovement()->MaxWalkSpeed = normalMovementSpeed_;
		break;
	case EMovementStatus::EMS_Sprinting:
		GetCharacterMovement()->MaxWalkSpeed = sprintMovementSpeed_;
		break;
	default:
		;
	}
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	updateStaminaAndMovementStatuses(DeltaTime);
	if (bIsAttacking_ && combatTarget_) {
		FRotator interpolatedRotatorToEnemy = FMath::RInterpTo(GetActorRotation(), getLookAtEnemyYawRotator(), DeltaTime, interpolateRotationSpeed_);
		SetActorRotation(interpolatedRotatorToEnemy);
	}
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);
	playerInputComponentReference = PlayerInputComponent;
	bindActionsAndAxes();

}

void AMainCharacter::moveForward(float value)
{
	if ((Controller != nullptr) && (value != 0.0f) && (!bIsAttacking_)) {
		bIsMovingForward_ = true;
		FRotator rotation = Controller->GetControlRotation();
		FRotator yawRotation(0.f, rotation.Yaw, 0.f);

		const FVector direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(direction, value);
	}
	else {
		bIsMovingForward_ = false;
	}
}

void AMainCharacter::moveRight(float value)
{
	if ((Controller != nullptr) && (value != 0.0f) && (!bIsAttacking_)) {
		bIsMovingRight_ = true;
		FRotator rotation = Controller->GetControlRotation();
		FRotator yawRotation(0.f, rotation.Yaw, 0.f);

		const FVector direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(direction, value);
	}
	else {
		bIsMovingRight_ = false;
	}
}

bool AMainCharacter::moves()
{
	return bIsMovingForward_ || bIsMovingRight_;
}

void AMainCharacter::sprintButtonPressed()
{
	if (staminaStatus_ == EStaminaStatus::ESS_Exhausted) {
		bIsSprinting_ = false;
	}
	else {
		bIsSprinting_ = true;
	}
}

void AMainCharacter::sprintButtonReleased()
{
	bIsSprinting_ = false;
}

void AMainCharacter::interactButtonPressed()
{
	bIsInteractButtonPressed_ = true;
	if (overlappedItem_) {
		AWeapon* overlappedWeapon = Cast<AWeapon>(overlappedItem_);
		if (overlappedWeapon) {
			overlappedWeapon->equip(this);
			overlappedItem_ = nullptr;
		}
	}
}

void AMainCharacter::interactButtonReleased()
{
	bIsInteractButtonPressed_ = false;
}

void AMainCharacter::pauseButtonPressed()
{
	AMainPlayerController* mainCharacterController = Cast<AMainPlayerController>(GetController());
	if (mainCharacterController) {
		mainCharacterController->togglePauseMenu();
	}
}

void AMainCharacter::pauseButtonReleased()
{
}

void AMainCharacter::setLoadingByUser(bool loadingByUser)
{
		UMyGameInstance* gameInstance = Cast<UMyGameInstance>(GetGameInstance());
		if (gameInstance) {
			gameInstance->bGameIsLoadingByUser = loadingByUser;
		}
}

bool AMainCharacter::getLoadingByUser()
{
		UMyGameInstance* gameInstance = Cast<UMyGameInstance>(GetGameInstance());
		if (gameInstance) {
			return gameInstance->bGameIsLoadingByUser;
		} else {
			return false;
		}
}

void AMainCharacter::setFirstGameStart(bool isFirstGameStart)
{
		UMyGameInstance* gameInstance = Cast<UMyGameInstance>(GetGameInstance());
		if (gameInstance) {
			gameInstance->bFirstGameStart = isFirstGameStart;
		}
}

bool AMainCharacter::firstGameStart()
{
		UMyGameInstance* gameInstance = Cast<UMyGameInstance>(GetGameInstance());
		if (gameInstance) {
			return gameInstance->bFirstGameStart;
		}
		return false;
}

void AMainCharacter::attack()
{
	// Check to not interrupt current attack animation
	if (!bIsAttacking_) {
		if (equippedWeapon_ && combatMontage_) {
			UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
			if (animInstance) {
				bIsAttacking_ = true;
				int32 attackVariant = FMath::RandRange(0, 1);
				switch (attackVariant) {
					case 0:
						animInstance->Montage_Play(combatMontage_, 1.7);
						animInstance->Montage_JumpToSection(FName("Attack_1"), combatMontage_);
						break;
					case 1:
						animInstance->Montage_Play(combatMontage_, 1.7);
						animInstance->Montage_JumpToSection(FName("Attack_2"), combatMontage_);
						break;
					default:
						;
				}
			}
		}
	}
	// Rotation to an enemy is implemented in Tick() function
}

void AMainCharacter::attackButtonPressed()
{
	bIsAttackButtonPressed_ = true;
	attack();
}

void AMainCharacter::attackButtonReleased()
{
	bIsAttackButtonPressed_ = false;
}

void AMainCharacter::attackEnd()
{
	bIsAttacking_ = false;
	// repeat attacks while attack is pressed down
	if (bIsAttackButtonPressed_) {
		attack();
	}
}

void AMainCharacter::setOverlappedItem(AItem* newOverlappedItem)
{
	overlappedItem_ = newOverlappedItem;
}

void AMainCharacter::turnAtRate(float rate)
{
	AddControllerYawInput(baseTurnRate * rate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::lookUpAtRate(float rate)
{
	AddControllerPitchInput(baseLookUpRate * rate * GetWorld()->GetDeltaSeconds());
}

float AMainCharacter::getNormalizedHealth() const
{
	return health_/maxHealth_;
}

float AMainCharacter::getNormalizedStamina() const
{
	return stamina_/maxStamina_;
}

int32 AMainCharacter::getCoins() const
{
	return coins_;
}

void AMainCharacter::increaseCoins(int32 numberOfCoins)
{
	coins_ += numberOfCoins;
}

void AMainCharacter::increaseHealth(float healthAmount)
{
	health_ = FMath::Clamp<int32>(health_ + healthAmount, 0, maxHealth_);
}

void AMainCharacter::setEquippedWeapon(AWeapon* newEquippedWeapon)
{
	if (equippedWeapon_) {
		equippedWeapon_->Destroy();
	}
	equippedWeapon_ = newEquippedWeapon;
}

AWeapon* AMainCharacter::getEquippedWeapon()
{
	return equippedWeapon_;
}

void AMainCharacter::die()
{
	clearBoundActionsAndAxes();
	setMovementStatus(EMovementStatus::EMS_Dead);
	UAnimInstance* mainCharAnimInstance = GetMesh()->GetAnimInstance();
	if (mainCharAnimInstance && combatMontage_) {
		mainCharAnimInstance->Montage_Play(combatMontage_, 1.f);
		mainCharAnimInstance->Montage_JumpToSection("Death", combatMontage_);
	}

	if (equippedWeapon_) {
		equippedWeapon_->deactivateCollision();
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// the death ends in the onDeathAnimationEnd method
}

void AMainCharacter::setCombatTarget(AEnemy* newCombatTarget)
{
	combatTarget_ = newCombatTarget;
}

AEnemy* AMainCharacter::getCombatTarget()
{
	return combatTarget_;
}

void AMainCharacter::setStaminaStatus(EStaminaStatus newStaminaStatus)
{
	staminaStatus_ = newStaminaStatus;
}

void AMainCharacter::updateStaminaAndMovementStatuses(float deltaTime)
{
	float deltaStamina = staminaDrainRate_ * deltaTime;
	if (movementStatus_ == EMovementStatus::EMS_Dead) { return; }

	switch (staminaStatus_) {
	// State when stamina amount is more than low level border (staminaLowLevel_)
	case EStaminaStatus::ESS_Normal:
		if (bIsSprinting_ && moves()) {
			setMovementStatus(EMovementStatus::EMS_Sprinting);
			updateStamina(deltaStamina);
		}
		else {
			setMovementStatus(EMovementStatus::EMS_Normal);
			updateStamina(deltaStamina);
		}

		if (stamina_ <= staminaLowLevel_) {
			setStaminaStatus(EStaminaStatus::ESS_Low);
		}
		break;
	// State when stamina amount is less than low level border (staminaLowLevel_)
	case EStaminaStatus::ESS_Low:
		if (bIsSprinting_) {
			setMovementStatus(EMovementStatus::EMS_Sprinting);
			updateStamina(deltaStamina);
		}
		else {
			setMovementStatus(EMovementStatus::EMS_Normal);
			updateStamina(deltaStamina);
		}

		if (stamina_ > staminaLowLevel_) {
			setStaminaStatus(EStaminaStatus::ESS_Normal);
		}
			
		if (stamina_ <= 0) {
			setStaminaStatus(EStaminaStatus::ESS_Exhausted);
		}

		break;
	// State when stamina amount is zero or lesser
	case EStaminaStatus::ESS_Exhausted:
		setMovementStatus(EMovementStatus::EMS_Normal);
		stamina_ += deltaStamina;
		if (stamina_ > staminaLowLevel_) {
			setStaminaStatus(EStaminaStatus::ESS_Normal);
		}
		break;
	default:
		;
	}

	if (stamina_ > maxStamina_) {
		stamina_ = maxStamina_;
	} else if (stamina_ < 0) {
		stamina_ = 0;
	}
}

FRotator AMainCharacter::getLookAtEnemyYawRotator()
{
	if (combatTarget_) {
		FRotator lookAtCombatTargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), combatTarget_->GetActorLocation());
		// Zero all components except Yaw
		lookAtCombatTargetRotation.Pitch = 0.f;
		lookAtCombatTargetRotation.Roll = 0.f;
		return lookAtCombatTargetRotation;
	}
	return FRotator(0.f);
}

bool AMainCharacter::isArmed()
{
	return equippedWeapon_ != nullptr;
}

float AMainCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	health_ -= DamageAmount;
	if (health_ <= 0.f) {
		die();
	}
	return DamageAmount;
}

void AMainCharacter::bindActionsAndAxes()
{
	if (playerInputComponentReference->AxisBindings.Num() == 0) {
		playerInputComponentReference->BindAction(TEXT("Pause"), IE_Pressed, this, &AMainCharacter::pauseButtonPressed);
		playerInputComponentReference->BindAction(TEXT("Pause"), IE_Released, this, &AMainCharacter::pauseButtonReleased);

		playerInputComponentReference->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
		playerInputComponentReference->BindAction(TEXT("Jump"), IE_Released, this, &ACharacter::StopJumping);

		playerInputComponentReference->BindAction(TEXT("Sprint"), IE_Pressed, this, &AMainCharacter::sprintButtonPressed);
		playerInputComponentReference->BindAction(TEXT("Sprint"), IE_Released, this, &AMainCharacter::sprintButtonReleased);

		playerInputComponentReference->BindAction(TEXT("Interact"), IE_Pressed, this, &AMainCharacter::interactButtonPressed);
		playerInputComponentReference->BindAction(TEXT("Interact"), IE_Released, this, &AMainCharacter::interactButtonReleased);

		playerInputComponentReference->BindAction(TEXT("Attack"), IE_Pressed, this, &AMainCharacter::attackButtonPressed);
		playerInputComponentReference->BindAction(TEXT("Attack"), IE_Released, this, &AMainCharacter::attackButtonReleased);

		playerInputComponentReference->BindAxis(TEXT("MoveForward"), this, &AMainCharacter::moveForward);
		playerInputComponentReference->BindAxis(TEXT("MoveRight"), this, &AMainCharacter::moveRight);

		// For mouse input
		playerInputComponentReference->BindAxis(TEXT("Turn"), this, &APawn::AddControllerYawInput);
		playerInputComponentReference->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);

		// for arrows input
		playerInputComponentReference->BindAxis(TEXT("TurnAtRate"), this, &AMainCharacter::turnAtRate);
		playerInputComponentReference->BindAxis(TEXT("LookUpAtRate"), this, &AMainCharacter::lookUpAtRate);
	}
}

void AMainCharacter::clearBoundActionsAndAxes() {
	playerInputComponentReference->ClearActionBindings();
	playerInputComponentReference->AxisBindings.Empty();
}

void AMainCharacter::onDeathAnimationEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
}

void AMainCharacter::updateCombatTarget()
{
	AEnemy* closestEnemy = findClosestEnemy();
	if (closestEnemy) {
		combatTarget_ = closestEnemy;
		combatTarget_->showHealth();
	}
}

AEnemy* AMainCharacter::findClosestEnemy()
{
	// Returns nullptr ?
	AEnemy* closestEnemy = nullptr;
	FVector characterLocation = GetActorLocation();
	GetOverlappingActors(overlappingEnemies, enemyFilter);
	if (overlappingEnemies.Num() > 0) {
		closestEnemy = Cast<AEnemy>(overlappingEnemies[0]);
		if (closestEnemy) {
			float closestDistance = (closestEnemy->GetActorLocation() - characterLocation).Size();
			float currentDistance = closestDistance;
			for (auto currentEnemy : overlappingEnemies) {
				if (Cast<AEnemy>(currentEnemy)) {
					currentDistance = (currentEnemy->GetActorLocation() - characterLocation).Size();
					if (closestDistance < currentDistance) {
						closestEnemy = Cast<AEnemy>(currentEnemy);
						closestDistance = currentDistance;
					}
				}
			}
		}
	}
	return closestEnemy;
}

void AMainCharacter::switchLevel(FName newLevelName)
{
	if (newLevelName == "") { return; }
	UWorld* world = GetWorld();
	if (world) {
		FString currentLevelName = UGameplayStatics::GetCurrentLevelName(world);
		FName currentLevelFName = *currentLevelName;
		UE_LOG(LogTemp, Warning, TEXT("CurrentLevelName = %s"), *currentLevelName);
		if (newLevelName != currentLevelFName) {
			UGameplayStatics::OpenLevel(world, newLevelName);
		}
	}
}

void AMainCharacter::saveGame(ESaveLoadType eSaveType)
{
	USimpleSaveGame* saveGameObject = Cast<USimpleSaveGame>(UGameplayStatics::CreateSaveGameObject(USimpleSaveGame::StaticClass()));
	if (!saveGameObject) { return; }

	saveGameObject->characterStats.coins = coins_;
	saveGameObject->characterStats.health = health_;
	saveGameObject->characterStats.maxHealth = maxHealth_;
	saveGameObject->characterStats.stamina = stamina_;
	saveGameObject->characterStats.maxStamina = maxStamina_;
	saveGameObject->characterStats.location = GetActorLocation();
	saveGameObject->characterStats.rotation = GetActorRotation();

	// save current level name
	UWorld* world = GetWorld();
	if (world) {
		FString currentLevelName = UGameplayStatics::GetCurrentLevelName(world);
		saveGameObject->characterStats.levelName = currentLevelName;
	}

	if (equippedWeapon_) {
		saveGameObject->characterStats.equippedWeaponName = equippedWeapon_->name;
	}
	
	FString saveGameSlotName = saveGameObject->playerName;
	if (eSaveType == ESaveLoadType::ESL_LevelTransition) {
		saveGameSlotName = " "; // A string with a space only is used as service string to save and load while level transitioning
		setLoadingByUser(false);
	}
	UGameplayStatics::SaveGameToSlot(saveGameObject, saveGameSlotName, saveGameObject->playerIndex);
}

void AMainCharacter::loadGame(ESaveLoadType eLoadType)
{
	USimpleSaveGame* loadGameObject = Cast<USimpleSaveGame>(UGameplayStatics::CreateSaveGameObject(USimpleSaveGame::StaticClass()));
	FString loadGameSlotName = loadGameObject->playerName;
	if (eLoadType == ESaveLoadType::ESL_User) {
		setLoadingByUser(true);
	}
	if (eLoadType == ESaveLoadType::ESL_LevelTransition) {
		loadGameSlotName = " "; // A string with a space only is used as service string to save and load while level transitioning
	}

	loadGameObject = Cast<USimpleSaveGame>(UGameplayStatics::LoadGameFromSlot(loadGameSlotName, loadGameObject->playerIndex));
	if (!loadGameObject) { return; }

	FString newLevelName = loadGameObject->characterStats.levelName;
	//	If we load after transitioning to a new level 
	//	we don't need to switchlevel and set location and rotation
	if ((!newLevelName.IsEmpty()) && (eLoadType == ESaveLoadType::ESL_User)) {
		switchLevel(FName(newLevelName));
		SetActorLocation(loadGameObject->characterStats.location);
		SetActorRotation(loadGameObject->characterStats.rotation);
	}

	coins_ = loadGameObject->characterStats.coins;
	health_ = loadGameObject->characterStats.health;
	maxHealth_ = loadGameObject->characterStats.maxHealth;
	stamina_ = loadGameObject->characterStats.stamina;
	maxStamina_ = loadGameObject->characterStats.maxStamina;


	if (movementStatus_ == EMovementStatus::EMS_Dead) {
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		setMovementStatus(EMovementStatus::EMS_Normal);
		StopAnimMontage();
		GetMesh()->bPauseAnims = false;
		GetMesh()->bNoSkeletonUpdate = false;
		bindActionsAndAxes();
	}

	spawnSavedWeapon(loadGameObject);
}

void AMainCharacter::spawnSavedWeapon(USimpleSaveGame* loadGameObject) {
	if (!loadGameObject->characterStats.equippedWeaponName.IsEmpty())
	{
		UWorld* world = GetWorld();
		if (world && weaponTypes) {
			AItemsStorage* weapons = world->SpawnActor<AItemsStorage>(weaponTypes);
			FString weaponName = loadGameObject->characterStats.equippedWeaponName;

			if (weapons->weaponsMap.Contains(weaponName)) {
				TSubclassOf<AWeapon> weaponToSpawn = weapons->weaponsMap[weaponName];
				AWeapon* weaponToEquip = world->SpawnActor<AWeapon>(weaponToSpawn);
				if (weaponToEquip) {
					weaponToEquip->equip(this);
				}
			}
		}
	} 

}

