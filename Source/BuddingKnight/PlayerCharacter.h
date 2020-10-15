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
	class USphereComponent* DetectionSphere;

	/** Attack Animation **/
	UPROPERTY(EditAnywhere,Category=Animation,meta=(AllowPrivateAccess="true"))
	TArray<class UAnimMontage*> Combo;	

	UPROPERTY(EditAnywhere,Category=Timing,meta=(AllowPrivateAccess="true"))
	float AttackWaitTime;

	FTimerHandle TimerAttack;

	UPawnMovementComponent*  MovementComponent;

	uint32 AttackCounter;

	//BPStates
	UPROPERTY(BlueprintReadWrite,meta=(AllowPrivateAccess="true"))
	bool bCanAttack;
	
	UPROPERTY(BlueprintReadWrite,meta=(AllowPrivateAccess="true"))
	bool bIsRolling;
	
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
	bool ChangeCameraBoomRotation {false};
	
	UPROPERTY()
	bool ChangeCameraBoomLength {false};

	UPROPERTY()
	bool ChangeCameraBoomOffSet{false};

	UPROPERTY()
	bool ChangeCameraFOV{false};

	bool ChangeCameraPitch{false};
	
	UPROPERTY()
	bool IsSwitchingTarget {false};

	UPROPERTY()
	bool DelayIsReset {false};

	UPROPERTY()
	bool DetectionSphereIsColliding {false};

	UPROPERTY()
	bool ResetDelay {false};


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera - Platform")
	float CameraBoomLengthPlatform {0};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera - Fight")
	float CameraBoomLengthFight {0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera - Platform")
	FVector CameraBoomOffSetPlatform{FVector{0,0,0}};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera - Fight")
	FVector CameraBoomOffSetFight{FVector{0,0,0}};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera - Platform")
	float CameraFOVPlatform{0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera - Fight")
	float CameraFOVFight{0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera - Platform")
	float CameraPitchPlatform{0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera - Fight")
	float CameraPitchFight{0};
	
	UPROPERTY()
	float AlphaCameraBoomLength {0};

	UPROPERTY()
	float AlphaCameraBoomRot {0};

	UPROPERTY()
	float AlphaCameraBoomOffSet {0};
	
	UPROPERTY()
	float AlphaCameraFOV {0};

	UPROPERTY()
	float AlphaCameraPitch {0};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float CameraBoomLengthTransitionSpeed {0};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float CameraBoomRotSpeed {0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float CameraBoomOffSetTransitionSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float CameraFOVSpeed {0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float CameraPitchSpeed {0};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	APawn * LockEnemy;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    TArray<APawn *> Enemies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Seed")
	int NbSeed{0};
	
	UPROPERTY()
	float DistancePlayerLockEnemy;
	
protected:
	
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
	
	void ResetCameraLock(const float);
	
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface



	UPROPERTY()
	FTimerHandle TimeHandleSoftLock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float DelaySoftLockCooldown {0};

	UFUNCTION()
	void DelaySoftLock();

	UFUNCTION()
	void ResetSoftLock();
	
	UPROPERTY()
	FRotator OldCameraBoomRotation;

	UPROPERTY()
	bool OldCameraBoomRotationIsSet{false};
	
	public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }


	virtual void Jump() override;
	
	/** Make a transition between CameraBoomLength and CameraBoomLengthAttack. */
	UFUNCTION()
	void CameraBoomLengthTransition();

	UFUNCTION()
    void CameraBoomOffSetTransition();

	UFUNCTION()
    void CameraFOVTransition();

	UFUNCTION()
    void CameraPitchTransition();
	
	UFUNCTION()
	void UpdateCamera();
	
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
						int32 OtherBodyIndex, bool bFromSweep,
						const FHitResult& SweepResult);

	UFUNCTION()
     void OnOverlapEnd(UPrimitiveComponent* OverlappedComp,
     				   AActor* OtherActor,
     				   UPrimitiveComponent* OtherComp,
     				   int32 OtherBodyIndex);
};