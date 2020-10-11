// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class UTimelineComponent;

UCLASS()
class BUDDINGKNIGHT_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** A collision sphere in order to detect enemy*/
	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereDetection;

	/** Attack Animation **/
	UPROPERTY(EditAnywhere,Category=Animation,meta=(AllowPrivateAccess="true"))
	TArray<class UAnimMontage*> Combo;	

	UPROPERTY(EditAnywhere,Category=Timing,meta=(AllowPrivateAccess="true"))
	float AttackWaitTime;

	FTimerHandle TimerAttack;

	//States
	UPROPERTY(BlueprintReadWrite,meta=(AllowPrivateAccess="true"))
	bool bCanAttack;
	UPROPERTY(BlueprintReadWrite,meta=(AllowPrivateAccess="true"))
	bool bIsRolling;

	uint32 AttackCounter;
	
public:
	virtual void Tick(float DeltaSeconds) override;
	APlayerCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;
	
	UPROPERTY()
	bool AttackCamera {false};
	
	UPROPERTY()
	bool IsSwitchingTarget {false};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
	float CameraBoomLengthAttack {0};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
	float CameraBoomLength {0};
	
	UPROPERTY()
	float AlphaCameraBoomLength {0};

	UPROPERTY()
	float AlphaCameraBoomRot {0};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float CameraBoomTransitionSpeed {0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float CameraBoomRotSpeed {0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	APawn * LockEnemy;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    TArray<APawn *> Enemies;

	UPROPERTY()
	float DistancePlayerLockEnemy;
	
protected:

	/** Resets HMD orientation in VR. */
	// Void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	* Called via input to turn at a given rate. 
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate. 
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

	void Attack();
	void StopAttack();

	void Dodge();
	void StopDodge();

	void SelectLeft();
	void StopSelectLeft();

	UFUNCTION(BlueprintCallable)
	void OnValidateAttack();
	
	UFUNCTION(BlueprintCallable)
	void OnResetCombo();

	virtual void BeginPlay() override;


	void SelectRight();
	void StopSelectRight();
	
	void ChangeCameraType();

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Make a transition between CameraBoomLength and CameraBoomLengthAttack. */
	UFUNCTION()
	void CameraTransition();

	/** Rotate the CameraBoom in the direction of LockEnemy.*/
	UFUNCTION()
    void CameraLock();

	/** Search the closest enemy in enemies.*/
	UFUNCTION()
    void SearchClosestEnemy();

	/** When the camera is in attack mode, the controller rotation look at LockEnemy.*/
	UFUNCTION()
    void SetControllerRotation() const;

	UFUNCTION()
    void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp,
    					class AActor* OtherActor,
    					class UPrimitiveComponent* OtherComp,
    					int32 OtherBodyIndex,
    					bool bFromSweep,
    					const FHitResult& SweepResult);
};
