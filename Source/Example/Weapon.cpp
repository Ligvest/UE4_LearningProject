// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "MainCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Enemy.h"



AWeapon::AWeapon() {
	skeletalMesh_ = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	skeletalMesh_->SetupAttachment(GetRootComponent());

	attackCollision_ = CreateDefaultSubobject<UBoxComponent>(TEXT("combatCollision"));
	attackCollision_->SetupAttachment(GetRootComponent());

	weaponStatus_ = EWeaponStatus::EWS_Pickup;
	equipSound_ = nullptr;

	damage_ = 50.f;
	
	weaponOwner_ = nullptr;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	attackCollision_->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::attackCollisionOnOverlapBegin);
	attackCollision_->OnComponentEndOverlap.AddDynamic(this, &AWeapon::attackCollisionOnOverlapEnd);

	attackCollision_->SetCollisionEnabled(ECollisionEnabled::NoCollision); // to not block and to get overlap notifications
	attackCollision_->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic); // set this object type to WorldDynamic
	attackCollision_->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore); // ignore all collisions
	attackCollision_->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap); // generate overlap event on collision with pawn
}

void AWeapon::equip(AMainCharacter* mainCharacter)
{
	if (mainCharacter) {
		skeletalMesh_->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		skeletalMesh_->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

		skeletalMesh_->SetSimulatePhysics(false);

		const USkeletalMeshSocket* weaponSocket = mainCharacter->GetMesh()->GetSocketByName("WeaponSocket");
		if (weaponSocket) {
			weaponSocket->AttachActor(this, mainCharacter->GetMesh());
			mainCharacter->setEquippedWeapon(this);
			bShouldRotate_ = false;
			if (equipSound_) {
				UGameplayStatics::SpawnSound2D(this, equipSound_);
			}
		}

		weaponOwner_ = mainCharacter;
	}
}

void AWeapon::onBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::onBeginOverlap(OverlappedComponent,  OtherActor, OtherComp, OtherBodyIndex,  bFromSweep, SweepResult);
	AMainCharacter* mainCharacter = Cast<AMainCharacter>(OtherActor);
	if ((getWeaponStatus() == EWeaponStatus::EWS_Pickup) && mainCharacter) {
		mainCharacter->setOverlappedItem(this);
	}
}

void AWeapon::onEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::onEndOverlap(OverlappedComponent,  OtherActor,  OtherComp,  OtherBodyIndex);
	AMainCharacter* mainCharacter = Cast<AMainCharacter>(OtherActor);
	if (mainCharacter) {
		mainCharacter->setOverlappedItem(nullptr);
	}
}

void AWeapon::attackCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor) {
		AEnemy* enemy = Cast<AEnemy>(OtherActor);
		if (enemy) {
			if (damageTypeClass_) {
				UGameplayStatics::ApplyDamage(enemy, damage_, getWeaponOwner()->GetController(), this, damageTypeClass_);
			}

			// TO MOVE TO AEnemy::TakeDamage(...)
			if (enemy->takeDamageParticles_) {
				const USkeletalMeshSocket* damageSocket = skeletalMesh_->GetSocketByName("DamageSocket");
				if (damageSocket) {
					FVector damageSocketLocation = damageSocket->GetSocketLocation(skeletalMesh_);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), enemy->takeDamageParticles_, damageSocketLocation);
					if (enemy->takeDamageSound_) {
						UGameplayStatics::SpawnSound2D(this, enemy->takeDamageSound_);
					}
				}
			}
		}
	}
}

void AWeapon::attackCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AWeapon::activateCollision()
{
	attackCollision_->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // to not block and to get overlap notifications
}

void AWeapon::deactivateCollision()
{
	attackCollision_->SetCollisionEnabled(ECollisionEnabled::NoCollision); // to not block anything and not notify any overlapping
}

void AWeapon::playAttackSound()
{
	UGameplayStatics::SpawnSound2D(this, attackSound_);
}

AMainCharacter* AWeapon::getWeaponOwner() const {
	return weaponOwner_;
}

