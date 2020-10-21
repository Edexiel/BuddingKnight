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
	class UBlackboardComponent* BB;
	
	UPROPERTY(EditAnywhere)
	float DetectionRadius;
	UPROPERTY(EditAnywhere)
	float MarginRadius;

	UPROPERTY(EditAnywhere)
	float DepopTime;
	
	
protected:
	virtual void BeginPlay() override;
};
