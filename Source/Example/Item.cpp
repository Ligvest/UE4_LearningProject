// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"
#include "MainCharacter.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	collisionVolume_ = CreateDefaultSubobject<USphereComponent>(TEXT("collisionVolume"));
	SetRootComponent(collisionVolume_);

	mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	mesh_->SetupAttachment(GetRootComponent());

	idleParticlesComponent_ = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("idleParticlesComponent"));
	idleParticlesComponent_->SetupAttachment(GetRootComponent());

	bShouldRotate_ = false;
	rotationRate_ = 45.f;
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	collisionVolume_->OnComponentBeginOverlap.AddDynamic(this, &AItem::onBeginOverlap);
	collisionVolume_->OnComponentEndOverlap.AddDynamic(this, &AItem::onEndOverlap);
	
}

void AItem::onBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// If you will need a item, that reacts to enemy,
	// then create 2 new inherited classes "AItemForCharacter" and "AItemForEnemy"
	// and move this body to the new "AItemForCharacter"
	AMainCharacter* mainCharacter = Cast<AMainCharacter>(OtherActor);
	if (mainCharacter) {
		if (overlapParticles_) {
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), overlapParticles_, GetActorLocation());
		}
		if (overlapSound_) {
			UGameplayStatics::SpawnSound2D(this, overlapSound_);
		}
	}
}

void AItem::onEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bShouldRotate_) {
		//FRotator rotator = GetActorRotation();
		//rotator.Yaw += rotationRate_ * DeltaTime;
		//SetActorRotation(rotator);
		AddActorLocalRotation(FRotator(0.f, rotationRate_ * DeltaTime, 0.f));
	}
}

