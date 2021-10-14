// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"


UENUM(BlueprintType)
enum class EWeaponStatus : uint8 {
	EWS_Pickup UMETA(DisplayName = "Pickup"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),

	EWS_MAX UMETA(DisplayName = "DefaultMax")
};


UCLASS()
class EXAMPLE_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();

	void equip(class AMainCharacter* mainCharacter);

	virtual void onBeginOverlap(UPrimitiveComponent* OverlappedComponent,
				AActor* OtherActor,
				UPrimitiveComponent* OtherComp,
				int32 OtherBodyIndex,
				bool bFromSweep,
				const FHitResult& SweepResult) override;

	virtual void onEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	FORCEINLINE const EWeaponStatus getWeaponStatus() const { return weaponStatus_; }
	void setWeaponStatus(EWeaponStatus newStatus);
		
	UFUNCTION()
	virtual void attackCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void attackCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void activateCollision();

	UFUNCTION(BlueprintCallable)
	void deactivateCollision();

	UFUNCTION(BlueprintCallable)
	void playAttackSound();

	UFUNCTION(BlueprintCallable, Category = "Status")
	AMainCharacter* getWeaponOwner() const;
	
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Data")
	FString name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	class USoundCue* equipSound_;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	class USoundCue* attackSound_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	EWeaponStatus weaponStatus_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class UBoxComponent* attackCollision_;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SkeletalMesh")
	class USkeletalMeshComponent* skeletalMesh_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float damage_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> damageTypeClass_;

protected:
	virtual void BeginPlay() override;

	class AMainCharacter* weaponOwner_;
};
