// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "AIC_EnemyCAC.generated.h"

UCLASS()
class BUDDINGKNIGHT_API AAIC_EnemyCAC : public AAIController
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	class UBehaviorTree* BTAsset;

	UPROPERTY()
	bool Init{false};
	
	UPROPERTY()
	class APlayerCharacter* PlayerCharacter;
	
	UPROPERTY(EditAnywhere)
	float AttackRange;

	UPROPERTY(EditAnywhere)
	float MarginRadius;

	UPROPERTY(EditAnywhere)
	float DepopTime;

	UPROPERTY(EditAnywhere)
	float MaxEnemiesOnPlayer;

	UPROPERTY(EditAnywhere)
	float MaxWalkSpeed;

	UPROPERTY(EditAnywhere)
	float WaitTimeBetweenAttacks;

	UPROPERTY()
	class AActor* Target{nullptr};


protected:
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;
public:
	
	UFUNCTION(BlueprintCallable)
	void SetTarget(AActor* Actor);
	
	virtual void Tick(float DeltaTime) override;
};
