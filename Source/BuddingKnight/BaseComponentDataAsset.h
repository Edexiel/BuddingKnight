// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BaseComponentDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class BUDDINGKNIGHT_API UBaseComponentDataAsset : public UDataAsset
{
	GENERATED_BODY()
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	float Health;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	float MaxHealth;

	
	
public:		
	UFUNCTION(BlueprintCallable)
        float GetHealth() const;
	
	UFUNCTION(BlueprintCallable)
        float GetMaxHealth() const;
};
