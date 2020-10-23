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

public:
	UCameraDataAsset();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera - Platform")
	float CameraBoomLengthPlatform;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera - Fight")
	float CameraBoomLengthFight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera - Platform")
	FVector CameraBoomOffSetPlatform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera - Fight")
	FVector CameraBoomOffSetFight;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera - Platform")
	float CameraFOVPlatform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera - Fight")
	float CameraFOVFight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera - Platform")
	float CameraPitchPlatform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera - Fight")
	float CameraPitchFight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float CameraBoomLengthTransitionSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float CameraBoomRotSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float CameraBoomOffSetTransitionSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float CameraFOVSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float CameraPitchSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float DelaySoftLockCooldown;
	
public:
	UFUNCTION()
	float GetCameraBoomLengthPlatform() const;
	
	UFUNCTION()
	float GetCameraBoomLengthFight() const;

	UFUNCTION()
	FVector GetCameraBoomOffSetPlatform() const;
	
	UFUNCTION()
	FVector GetCameraBoomOffSetFight() const;

	UFUNCTION()
	float GetCameraFOVPlatform() const;

	UFUNCTION()
	float GetCameraFOVFight() const;

	UFUNCTION()
	float GetCameraPitchPlatform() const;
	
	UFUNCTION()
	float GetCameraPitchFight() const;

	UFUNCTION()
	float GetCameraBoomLengthTransitionSpeed() const;

	UFUNCTION()
	float GetCameraBoomRotSpeed() const;

	UFUNCTION()
	float GetCameraBoomOffSetTransitionSpeed() const;

	UFUNCTION()
	float GetCameraFOVSpeed() const;

	UFUNCTION()
	float GetCameraPitchSpeed() const;

	UFUNCTION()
	float GetDelaySoftLockCooldown() const;
};
