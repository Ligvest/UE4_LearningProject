// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingPlatform.h"
#include "Math/UnrealMathUtility.h"
#include "Components/BoxComponent.h"

// Sets default values
AFloatingPlatform::AFloatingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true; 
	PrimaryActorTick.bStartWithTickEnabled = false; 
	// enable tick in standBoxTriggerOnOverlapBegin


	mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(mesh_);

	startPoint_ = FVector(0.f);
	endPoint_ = FVector(0.f);

	interpSpeed_ = 1.f;
	delayBetweenTogglingDirections_ = 1.f;

	standBoxTrigger_ = CreateDefaultSubobject<UBoxComponent>(TEXT("StandBoxTrigger"));
	standBoxTrigger_->SetupAttachment(GetRootComponent());

	bChangingDirection = false;
}

// Called when the game starts or when spawned
void AFloatingPlatform::BeginPlay()
{
	Super::BeginPlay();

	startPoint_ = GetActorLocation();
	endPoint_ = startPoint_ + endPoint_; // Converting relative coordinates of endPoint_ to world coordinates
	standBoxTrigger_->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	standBoxTrigger_->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	standBoxTrigger_->OnComponentBeginOverlap.AddDynamic(this, &AFloatingPlatform::standBoxTriggerOnOverlapBegin);
	SetActorTickEnabled(false);
}

void AFloatingPlatform::updateLocation(float deltaTime)
{
	FVector destination = FMath::VInterpTo(GetActorLocation(), endPoint_, deltaTime, interpSpeed_);
	SetActorLocation(destination);
}

void AFloatingPlatform::swapVectors(FVector& first, FVector& second)
{
	FVector temp(first);
	first = second;
	second = temp;
}

void AFloatingPlatform::changeMoveDirection()
{
		swapVectors(startPoint_, endPoint_);
		SetActorTickEnabled(false);
		bChangingDirection = false;
}

void AFloatingPlatform::standBoxTriggerOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	SetActorTickEnabled(true);
}

// Called every frame
void AFloatingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	updateLocation(DeltaTime);
	if (((GetActorLocation() - endPoint_).Size()) < 50.f) {
		if (!bChangingDirection) {
			GetWorldTimerManager().SetTimer(changeDirectionTimerHandle, this, &AFloatingPlatform::changeMoveDirection, 1.f);//delayBetweenTogglingDirections_);
			bChangingDirection = true;
		}
	}
}



