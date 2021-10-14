// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "EnemyAnimInstance.h"
#include "MainCharacter.h"
#include "MainPlayerController.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "AIController.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"

AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	agroSphere_ = CreateDefaultSubobject<USphereComponent>("agroSphere");
	agroSphere_->SetupAttachment(GetRootComponent());
	agroSphere_->InitSphereRadius(600.f);
	
	combatSphere_ = CreateDefaultSubobject<USphereComponent>("combatSphere");
	combatSphere_->SetupAttachment(GetRootComponent());
	combatSphere_->InitSphereRadius(100.f);

	attackCollision_ = CreateDefaultSubobject<UBoxComponent>("attackCollision");
	attackCollision_->SetupAttachment(GetMesh(), "AttackCollisionSocket");

	bIsMainCharacterInCombatSphere_ = false;
	bIsMainCharacterInAgroSphere_ = false;
	combatTarget_ = nullptr;

	health_ = 100.f;
	maxHealth_ = 100.f;
	damage_ = 10.f;
	bIsAttacking_ = false;

	minAttackDelay_ = 0.4;
	maxAttackDelay_ = 1.5;

	attackSpeed_ = 1.f;

	removeCorpseDelay_ = 3.5f;

	setMovementStatus(EEnemyMovementStatus::EMS_Idle);

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	controller_ = Cast<AAIController>(GetController());

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	agroSphere_->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::agroSphereOnOverlapBegin);
	agroSphere_->OnComponentEndOverlap.AddDynamic(this, &AEnemy::agroSphereOnOverlapEnd);

	combatSphere_->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::combatSphereOnOverlapBegin);
	combatSphere_->OnComponentEndOverlap.AddDynamic(this, &AEnemy::combatSphereOnOverlapEnd);

	attackCollision_->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::attackCollisionOnOverlapBegin);
	attackCollision_->OnComponentEndOverlap.AddDynamic(this, &AEnemy::attackCollisionOnOverlapEnd);

	attackCollision_->SetCollisionEnabled(ECollisionEnabled::NoCollision); // to not block and to get overlap notifications
	attackCollision_->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic); // set this object type to WorldDynamic
	attackCollision_->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore); // ignore all collisions
	attackCollision_->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap); // generate overlap event on collision with pawn
}

// Called from EnemyAnim blueprint on "AttackEnd" notification
void AEnemy::attackEnd()
{
	if (isAlive())/* is it nessesary?*/ {
		if (!bIsMainCharacterInCombatSphere_) {
			setMovementStatus(EEnemyMovementStatus::EMS_Moves);
			moveToTarget(combatTarget_);
		}
		else {
			float attackDelay = FMath::FRandRange(minAttackDelay_, maxAttackDelay_);
			GetWorld()->GetTimerManager().SetTimer(attackDelayTimerHandle_, this, &AEnemy::attack, attackDelay);
		}
	}
}

void AEnemy::attack()
{
	if (isAlive()) {
		stopChasing();
		setMovementStatus(EEnemyMovementStatus::EMS_Attacks);
		bIsAttacking_ = true;
		UAnimInstance* enemyAnimInstance = GetMesh()->GetAnimInstance();
		if (enemyAnimInstance) {
			enemyAnimInstance->Montage_Play(combatMontage_, attackSpeed_);
			enemyAnimInstance->Montage_JumpToSection("Attack", combatMontage_);
		}
	}
}

void AEnemy::attackCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor) {
		AMainCharacter* mainCharacter = Cast<AMainCharacter>(OtherActor); 
		if (mainCharacter) {
			if (mainCharacter->takeDamageParticles_) {
				const USkeletalMeshSocket* damageSocket = GetMesh()->GetSocketByName("AttackCollisionSocket");
				if (damageSocket) {
					FVector damageSocketLocation = damageSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), mainCharacter->takeDamageParticles_, damageSocketLocation);
					if (mainCharacter->takeDamageSound_) {
						UGameplayStatics::SpawnSound2D(this, mainCharacter->takeDamageSound_);
					}
				}
			}
			UGameplayStatics::ApplyDamage(mainCharacter, damage_, GetController(), this, damageTypeClass_);
		}
	}

}

void AEnemy::attackCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AEnemy::activateAttackCollision()
{
	attackCollision_->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // to not block and to get overlap notifications
}

void AEnemy::deactivateAttackCollision()
{
	attackCollision_->SetCollisionEnabled(ECollisionEnabled::NoCollision); // disable collisions
}

void AEnemy::stopChasing()
{
	if (controller_ && isAlive()/* is it nessesary?*/) {
		controller_->StopMovement();
	}
}

void AEnemy::die()
{
	hideHealth();
	setMovementStatus(EEnemyMovementStatus::EMS_Dead);
	UAnimInstance* enemyAnimInstance = GetMesh()->GetAnimInstance();
	if (enemyAnimInstance && combatMontage_) {
		enemyAnimInstance->Montage_Play(combatMontage_, 1.f);
		enemyAnimInstance->Montage_JumpToSection("Death", combatMontage_);
	}

	agroSphere_->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	combatSphere_->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	attackCollision_->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (combatTarget_) {
		// After death let to update a combat target to 
		// mainChatacter to see healthbar of another enemies
		combatTarget_->updateCombatTarget();
	}

	// the death ends in the onDeathAnimationEnd method
}

void AEnemy::removeCorpse()
{
	this->Destroy();
}

void AEnemy::showHealth()
{
	if (combatTarget_) {
		AMainCharacter* mainCharacter = combatTarget_;
		AMainPlayerController* mainCharacterController = Cast<AMainPlayerController>(mainCharacter->GetController());
		if (mainCharacterController) {
			mainCharacterController->showEnemyHealthBar(this);
		}
	}
}

void AEnemy::hideHealth()
{
	if (combatTarget_) {
		AMainCharacter* mainCharacter = combatTarget_;
		AMainPlayerController* mainCharacterController = Cast<AMainPlayerController>(mainCharacter->GetController());
		if (mainCharacterController) {
			mainCharacterController->hideEnemyHealthBar();
		}
	}
}

void AEnemy::clearCombatTargetLinks()
{
	if (combatTarget_) {
		AMainCharacter* mainCharacter = combatTarget_;
		if (mainCharacter->getCombatTarget() == this) {
			//mainCharacter->setCombatTarget(nullptr); // this crashes the game when killing an enemy
		}

	//	AMainPlayerController* mainCharacterController = Cast<AMainPlayerController>(mainCharacter->GetController());
	//	if (mainCharacterController) {
	//		mainCharacterController->hideEnemyHealthBar();
	//	}
	}
}


void AEnemy::moveToTarget(AMainCharacter* mainCharacter)
{
	setMovementStatus(EEnemyMovementStatus::EMS_Moves);

	if (controller_ && isAlive()/* is it nessesary?*/) {
		controller_->MoveToActor(mainCharacter, 10.f);
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::agroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMainCharacter* mainCharacter = Cast<AMainCharacter>(OtherActor);
	if (mainCharacter) {
		bIsMainCharacterInAgroSphere_ = true;
		moveToTarget(mainCharacter);
	}
}

void AEnemy::agroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMainCharacter* mainCharacter = Cast<AMainCharacter>(OtherActor);
	if (mainCharacter) {
		stopChasing();
		hideHealth();
		mainCharacter->setCombatTarget(nullptr);
		bIsMainCharacterInAgroSphere_ = false;
	}
}

void AEnemy::combatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMainCharacter* mainCharacter = Cast<AMainCharacter>(OtherActor);
	if (mainCharacter) {
		combatTarget_ = mainCharacter;
		bIsMainCharacterInCombatSphere_ = true;
		mainCharacter->setCombatTarget(this);
		showHealth();
		attack();
	}
}

void AEnemy::combatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor) {
		AMainCharacter* mainCharacter = Cast<AMainCharacter>(OtherActor);
		if (mainCharacter) {
			GetWorldTimerManager().ClearTimer(attackDelayTimerHandle_);
			bIsMainCharacterInCombatSphere_ = false;
			if (bIsMainCharacterInAgroSphere_) {
				moveToTarget(mainCharacter);
			}
		}
	}
}

void AEnemy::playAttackSound()
{
	if (attackSound_) {
		UGameplayStatics::SpawnSound2D(this, attackSound_);
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	health_ -= DamageAmount;
	if (health_ <= 0.f) {
		AMainCharacter* mainCharacter = Cast<AMainCharacter>(DamageCauser);
		if (mainCharacter) {
			mainCharacter->setCombatTarget(nullptr);
		}
		die();
	}
	return DamageAmount;
}

void AEnemy::onDeathAnimationEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
	GetWorldTimerManager().SetTimer(removeCorpseTimerHandle_, this, &AEnemy::removeCorpse, removeCorpseDelay_);
	//clearCombatTargetLinks();
}

bool AEnemy::isAlive()
{
	return movementStatus_ != EEnemyMovementStatus::EMS_Dead;
}

float AEnemy::getNormalizedHealth() const
{
	return health_/maxHealth_;
}
