// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseComponent.h"
#include "AIController.h"
#include "Entity.generated.h"

UCLASS()
class BUDDINGKNIGHT_API AEntity : public AAIController 
{
	GENERATED_BODY()
public:
	AEntity();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "BaseComponent", meta = (AllowPrivateAccess = true))
		class UBaseComponent* BaseComponent;
	
};
