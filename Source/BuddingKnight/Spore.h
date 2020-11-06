// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Plant.h"
#include "Spore.generated.h"

UCLASS()
class BUDDINGKNIGHT_API ASpore : public APlant
{
	GENERATED_BODY()
	
protected:
	virtual void Special() override;
	
};
