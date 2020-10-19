// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CameraDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class BUDDINGKNIGHT_API UCameraDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera - Platform")
	float CameraBoomLengthPlatform {450};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera - Fight")
	float CameraBoomLengthFight {600};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera - Platform")
	FVector CameraBoomOffSetPlatform{FVector{0,70,80}};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera - Fight")
	FVector CameraBoomOffSetFight{FVector{0,70,80}};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera - Platform")
	float CameraFOVPlatform{90};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera - Fight")
	float CameraFOVFight{110};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera - Platform")
	float CameraPitchPlatform{0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera - Fight")
	float CameraPitchFight{0};
	
public:
	UFUNCTION()
	float GetCameraBoomLengthPlatform();
};
