// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Plant.h"
#include "Tree.generated.h"


UCLASS()
class BUDDINGKNIGHT_API ATree : public APlant
{
	GENERATED_BODY()
public:
	ATree();

	virtual void BeginPlay() override;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	class UBoxComponent* AttackBox;
	
};
