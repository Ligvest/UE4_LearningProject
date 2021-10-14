// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class EXAMPLE_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// A variable to let UE know what type of user created widget we want to add
	// It's specified in blueprints
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	TSubclassOf<class UUserWidget>HudOverlayAsset_;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	class UUserWidget* HudOverlay_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	TSubclassOf<class UUserWidget>enemyHealthBarAsset_;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Widgets)
	class UUserWidget* enemyHealthBar_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	TSubclassOf<class UUserWidget> pauseMenuWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Widgets)
	class UUserWidget* pauseMenu;


public:
	void showEnemyHealthBar(class AEnemy* enemy);
	void hideEnemyHealthBar();
	void updateEnemyHealthBarLocation();

	bool bPauseMenuHidden;
	UFUNCTION(BlueprintNativeEvent, Category = "HUD")
	void showPauseMenu();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD")
	void hidePauseMenu();
	void togglePauseMenu();


protected:
	AEnemy* enemy_;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
};
