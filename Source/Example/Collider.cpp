// Fill out your copyright notice in the Description page of Project Settings.


#include "Collider.h"

// Sets default values
ACollider::ACollider()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	sphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("sphereComponent"));
	SetRootComponent(sphereComponent);
	sphereComponent->InitSphereRadius(40.f);
	sphereComponent->SetCollisionProfileName(TEXT("Pawn"));

	meshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("meshComponent"));
	meshComponent->SetupAttachment(GetRootComponent());

	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshComponentAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
	if (meshComponentAsset.Succeeded()) {
		meshComponent->SetStaticMesh(meshComponentAsset.Object);
		meshComponent->SetRelativeLocation(FVector(0.f, 0.f, -40.f));
		meshComponent->SetRelativeScale3D(FVector(0.8f));
	}
	
	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("springArm"));
	springArm->SetupAttachment(GetRootComponent());
	springArm->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
	springArm->TargetArmLength = 400.f;
	springArm->bEnableCameraLag = true;
	springArm->CameraLagSpeed = 3.f;


	cameraInput = FVector2D(0.f);
	cameraRotator = FRotator(0.f);
	cameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("cameraComponent"));
	cameraComponent->SetupAttachment(springArm, USpringArmComponent::SocketName);

	ourMovementComponent = CreateDefaultSubobject<UColliderMovementComponent>(TEXT("ourMovementComponent"));
	ourMovementComponent->UpdatedComponent = GetRootComponent(); 

	speedMultiplier = 10.f;

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ACollider::BeginPlay()
{
	Super::BeginPlay();
}

void ACollider::moveForward(float value)
{
	value *= speedMultiplier;
	FVector forward = GetActorForwardVector();
	AddMovementInput(forward, value); // didn't replace
}

void ACollider::moveRight(float value)
{
	value *= speedMultiplier;
	FVector right = GetActorRightVector();
	AddMovementInput(right, value);
}

void ACollider::yawCamera(float axisValue)
{
	cameraInput.X = axisValue;
	//cameraRotator.Yaw = axisValue;
}

void ACollider::pitchCamera(float axisValue)
{
	cameraInput.Y = axisValue;
	//cameraRotator.Pitch = axisValue;
}


UPawnMovementComponent* ACollider::GetMovementComponent() const
{
	return ourMovementComponent;
}

// Called every frame
void ACollider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// cameraInput.Y - Pitch, cameraInput.X - Yaw
	// Yaw the collider
	AddActorLocalRotation(FRotator(0.f, cameraInput.X, 0.f));

	// Pitch springArm
	FRotator newSpringArmRotation = springArm->GetComponentRotation();
	newSpringArmRotation.Pitch = FMath::Clamp(cameraInput.Y + newSpringArmRotation.Pitch, -80.f, -5.f);
	springArm->SetWorldRotation(newSpringArmRotation);
}

// Called to bind functionality to input
void ACollider::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ACollider::moveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ACollider::moveRight);
	PlayerInputComponent->BindAxis(TEXT("CameraPitch"), this, &ACollider::pitchCamera);
	PlayerInputComponent->BindAxis(TEXT("CameraYaw"), this, &ACollider::yawCamera);

}

