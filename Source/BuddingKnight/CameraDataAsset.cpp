// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraDataAsset.h"

UCameraDataAsset::UCameraDataAsset()
{
    CameraBoomLengthPlatform = 0;
    CameraBoomLengthFight = 0;

    CameraBoomOffSetPlatform = FVector(0,0,0);
    CameraBoomOffSetFight = FVector(0,0,0);
    
    CameraFOVPlatform = 0;
    CameraFOVFight = 0;
    
    CameraPitchPlatform = 0;
    CameraPitchFight = 0;
    
    CameraBoomLengthTransitionSpeed = 0;
    CameraBoomRotSpeed = 0;
    CameraFOVSpeed = 0;
    CameraBoomLengthTransitionSpeed = 0;
    CameraPitchSpeed = 0;
}

float UCameraDataAsset::GetCameraBoomLengthPlatform() const
{
    return CameraBoomLengthPlatform;
}

float UCameraDataAsset::GetCameraBoomLengthFight() const
{
    return CameraBoomLengthFight;
}

FVector UCameraDataAsset::GetCameraBoomOffSetPlatform() const
{
    return CameraBoomOffSetPlatform;
}

FVector UCameraDataAsset::GetCameraBoomOffSetFight() const
{
    return CameraBoomOffSetFight;
}

float UCameraDataAsset::GetCameraFOVPlatform() const
{
    return CameraFOVPlatform;
}

float UCameraDataAsset::GetCameraFOVFight() const
{
    return CameraFOVFight;
}

float UCameraDataAsset::GetCameraPitchPlatform() const
{
    return CameraPitchPlatform;
}

float UCameraDataAsset::GetCameraPitchFight() const
{
    return CameraPitchFight;
}

float UCameraDataAsset::GetCameraBoomLengthTransitionSpeed() const
{
    return CameraBoomLengthTransitionSpeed;
}

float UCameraDataAsset::GetCameraBoomRotSpeed() const
{
    return CameraBoomRotSpeed;
}

float UCameraDataAsset::GetCameraBoomOffSetTransitionSpeed() const
{
    return CameraBoomOffSetTransitionSpeed;
}

float UCameraDataAsset::GetCameraFOVSpeed() const
{
    return CameraFOVSpeed;
}

float UCameraDataAsset::GetCameraPitchSpeed() const
{
    return CameraPitchSpeed;
}

float UCameraDataAsset::GetDelaySoftLockCooldown() const
{
    return DelaySoftLockCooldown;
}

float UCameraDataAsset::GetLockAngle() const
{
    return LockAngle;
}



