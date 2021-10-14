// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Enemy.h"
#include "Components/CapsuleComponent.h"
#include "MainCharacter.h"

void AMainPlayerController::showEnemyHealthBar(AEnemy* newEnemy)
{
	enemy_ = newEnemy;
	enemyHealthBar_->SetVisibility(ESlateVisibility::Visible);
}

void AMainPlayerController::hideEnemyHealthBar()
{
	enemy_ = nullptr;
	enemyHealthBar_->SetVisibility(ESlateVisibility::Hidden);
}

void AMainPlayerController::updateEnemyHealthBarLocation()
{
	if (enemy_) {
		FVector enemyLocation = enemy_->GetActorLocation();
		FVector2D onScreenEnemyLocation;
		ProjectWorldLocationToScreen(enemyLocation, onScreenEnemyLocation);
		FVector2D healthBarLocation = onScreenEnemyLocation - enemy_->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		enemyHealthBar_->SetPositionInViewport(healthBarLocation);
		enemyHealthBar_->SetDesiredSizeInViewport(FVector2D(300.f, 20.f));

	}
}

void AMainPlayerController::showPauseMenu_Implementation()
{
	if (pauseMenu) {
		AMainCharacter* mainCharacter = Cast<AMainCharacter>(GetPawn());
		if (mainCharacter) {
			mainCharacter->clearBoundActionsAndAxes();
		}
		bPauseMenuHidden = false;
		SetInputMode(FInputModeGameAndUI{});
		SetShowMouseCursor(true);
		pauseMenu->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainPlayerController::hidePauseMenu_Implementation()
{
	if (pauseMenu) {
		bPauseMenuHidden = true;
		pauseMenu->SetVisibility(ESlateVisibility::Hidden);
		SetInputMode(FInputModeGameOnly{});
		SetShowMouseCursor(false);
		AMainCharacter* mainCharacter = Cast<AMainCharacter>(GetPawn());
		if (mainCharacter) {
			mainCharacter->bindActionsAndAxes();
		}
	}
}

void AMainPlayerController::togglePauseMenu()
{
	if (bPauseMenuHidden) {
		showPauseMenu();
	} else {
		hidePauseMenu();
	}
}

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	enemy_ = nullptr;

	if (HudOverlayAsset_) {
		HudOverlay_ = CreateWidget(this, HudOverlayAsset_);
		if (HudOverlay_) {
			HudOverlay_->AddToViewport();
			HudOverlay_->SetVisibility(ESlateVisibility::Visible);
		}
	}

	if (enemyHealthBarAsset_) {
		enemyHealthBar_ = CreateWidget(this, enemyHealthBarAsset_);
		if (enemyHealthBar_) {
			enemyHealthBar_->AddToViewport();
			enemyHealthBar_->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (pauseMenuWidget) {
		pauseMenu = CreateWidget(this, pauseMenuWidget);
		if (pauseMenu) {
			pauseMenu->AddToViewport();
			pauseMenu->SetVisibility(ESlateVisibility::Hidden);
			bPauseMenuHidden = true;
		}
	}
}

void AMainPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	updateEnemyHealthBarLocation();
}
