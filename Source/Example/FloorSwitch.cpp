// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorSwitch.h"
#include "Components/BoxComponent.h"

// Sets default values
AFloorSwitch::AFloorSwitch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	triggerBox_ = CreateDefaultSubobject<UBoxComponent>(TEXT("triggerBox"));
	SetRootComponent(triggerBox_);
	triggerBox_->SetCollisionEnabled(ECollisionEnabled::QueryOnly); 

    // Changes the collision channel that this object uses
	triggerBox_->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic); // ECC - E Collision Channel 
	triggerBox_->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore); // Ignore all overlap events, 
	// except one that we specify in the next line.

	// React only on Pawn overlapping the trigger box
	triggerBox_->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap); // ECR - E Collision Response

	// Change the box extent size. This is the unscaled size, before component scale is applied.
	triggerBox_->SetBoxExtent(FVector(62.f, 62.f, 32.f));

	floorSwitch_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("floorSwitch"));
	floorSwitch_->SetupAttachment(GetRootComponent());

	door_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("door"));
	door_->SetupAttachment(GetRootComponent());

	// seconds
	switchTimeDelay = 1.f;
	bIsCharacterOnFloorSwitch = false;
}

void AFloorSwitch::switchTimerHasFired()
{
	if (!bIsCharacterOnFloorSwitch) {
		eRaiseFloorSwitch();
		eLowerDoor(); // unhandled event
	}
}

// Called when the game starts or when spawned
void AFloorSwitch::BeginPlay()
{
	Super::BeginPlay();

	triggerBox_->OnComponentBeginOverlap.AddDynamic(this, &AFloorSwitch::onOverlapBegin);
	triggerBox_->OnComponentEndOverlap.AddDynamic(this, &AFloorSwitch::onOverlapEnd);

	initialFloorSwitchLocation_  = floorSwitch_->GetComponentLocation();
	initialDoorLocation_ = door_->GetComponentLocation();
	
}

// Called every frame
void AFloorSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFloorSwitch::onOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Begin overlap"));
	if (!bIsCharacterOnFloorSwitch) { bIsCharacterOnFloorSwitch = true; }
	eLowerFloorSwitch();
	eRaiseDoor(); // unhandled event
}

void AFloorSwitch::onOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("End overlap"));
	if (bIsCharacterOnFloorSwitch) { bIsCharacterOnFloorSwitch = false; }
	GetWorldTimerManager().SetTimer(switchTimerHandle, this, &AFloorSwitch::switchTimerHasFired, switchTimeDelay);
}

void AFloorSwitch::addHeightToDoorInitialLocation(float heightToAdd)
{
	FVector newDoorLocation = initialDoorLocation_;
	newDoorLocation.Z += heightToAdd;
	door_->SetWorldLocation(newDoorLocation);
}

void AFloorSwitch::subtractHeightFromSwitchInitialLocation(float heightToSubtract)
{
	FVector newSwitchLocation = initialFloorSwitchLocation_;
	newSwitchLocation.Z -= heightToSubtract;
	floorSwitch_->SetWorldLocation(newSwitchLocation);
}

