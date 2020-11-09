// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Seed.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class UTimelineComponent;

UCLASS()
class BUDDINGKNIGHT_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* AudioComponent;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent * RightWeapon;

	/** Attack Animation **/
	UPROPERTY(EditAnywhere,Category=Animation,meta=(AllowPrivateAccess="true"))
	TArray<class UAnimMontage*> Combo;	

	/** Stun Animation **/
	UPROPERTY(EditAnywhere,Category=Animation,meta=(AllowPrivateAccess="true"))
	class UAnimMontage* StunAnimation;

	/** Stun sound **/
	UPROPERTY(EditAnywhere,Category=Sound,meta=(AllowPrivateAccess="true"))
	class USoundBase* StunSound;
	
	/** Stun Hit Animation **/
	UPROPERTY(EditAnywhere,Category=Animation,meta=(AllowPrivateAccess="true"))
	class UAnimMontage* GetHitAnimation;

	/** Hit sound **/
	UPROPERTY(EditAnywhere,Category=Sound,meta=(AllowPrivateAccess="true"))
	class USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category=Camera,meta=(AllowPrivateAccess="true"))
	class UCameraDataAsset* DataAssetCamera;
	
	/** Reference to MovementComponent **/
	class UPawnMovementComponent*  MovementComponent;

	/**	Attack counter for combo **/
	uint32 AttackCounter{0};

	/** Max hit before being totally KO **/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess="true"))
	int MaxHitReceived;

	/** Duration of stun **/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess="true"))
	float StunTime;

	/** Duration of hit stun **/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess="true"))
	float HitStunTime;
	
	/** Duration of knock out **/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess="true"))
	float KnockOutForce;
	
	int HitReceivedCounter{0};
	
	//BPStates
	UPROPERTY(BlueprintReadWrite,meta=(AllowPrivateAccess="true"))
	bool bCanAttack;
	
	UPROPERTY(BlueprintReadWrite,meta=(AllowPrivateAccess="true"))
	bool bIsRolling;

	UPROPERTY(EditAnywhere)
	float BaseDamage;

	UPROPERTY()
	float BonusDamage;

	UPROPERTY()
	AActor* OverlapActor;

	FTimerHandle StunHandle;
	FTimerHandle HitHandle;

	UFUNCTION()
	void ResetCanAttack();


	
	
public:
	APlayerCharacter();
	
	virtual void Tick(float DeltaSeconds) override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;
	
	UPROPERTY()
	bool ChangeCameraBoomRotation {false};
	
	UPROPERTY()
	bool IsInFightingMod {false};

	UPROPERTY()
	bool DelayIsReset {true};

	UPROPERTY()
	bool DetectionSphereIsColliding {false};

	UPROPERTY()
	bool ResetDelay {false};
	
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	APawn * LockEnemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	TArray<APawn *> Enemies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Seed")
	int NbTreeSeed{0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Seed")
	int NbLianaSeed{0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Seed")
	int NbSporeSeed{0};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess="true"), Category = "Seed")
	TEnumAsByte<EPlantType> TypeOfPlant;

	UPROPERTY()
	bool IsUsingSpecial;
	
	UPROPERTY()
	float DistancePlayerLockEnemy;

	UPROPERTY()
	class APot * ClosestPot{nullptr};
	
	/* Public functions */
	UFUNCTION(BlueprintGetter)
	bool IsTargetingPlayer(APawn* Pawn) const;

	UFUNCTION()
	void RegisterEnemy(APawn* Pawn, int Max);

	UFUNCTION()
	void UnregisterEnemy(APawn* Pawn);

	UFUNCTION()
	void SetBonusDamage(float Bonus);
	
	UFUNCTION()
	void UnsetBonusDamage();
	
	UFUNCTION()
	float GetBaseDamage() const;
	
	UFUNCTION()
	float GetDamage() const;

	
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
	void SelectLeft();
	void SelectRight();

	UFUNCTION(BlueprintCallable)
	void OnValidateAttack();
	
	UFUNCTION(BlueprintCallable)
	void OnResetCombo();

	virtual void BeginPlay() override;

	void ResetCameraLock(const float);
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY()
	FTimerHandle TimeHandleSoftLock;

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
	void CameraBoomLengthTransition(const float DeltaTime);

	UFUNCTION()
    void CameraBoomOffSetTransition(const float DeltaTime);

	UFUNCTION()
    void CameraFOVTransition(const float DeltaTime);

	UFUNCTION()
	void CameraPitchTransition(float DeltaTime);
	
	UFUNCTION()
	void UpdateCamera(const float DeltaTime);
	
	/** Rotate the CameraBoom in the direction of LockEnemy.*/
	UFUNCTION()
    void CameraLock(const float DeltaTime);

	/** Search the closest enemy in enemies.*/
	UFUNCTION()
    void SearchClosestEnemy();
	
	UFUNCTION(BlueprintCallable)
    void UseSeed();
	
	UFUNCTION()
    void OnCapsuleBeginOverlap(class UPrimitiveComponent* OverlappedComp,
    					class AActor* OtherActor,
    					class UPrimitiveComponent* OtherComp,
    					int32 OtherBodyIndex,
    					bool bFromSweep,
    					const FHitResult& SweepResult);

	UFUNCTION()
    void OnCapsuleEndOverlap(UPrimitiveComponent* OverlappedComp,
         			   AActor* OtherActor,
         			   UPrimitiveComponent* OtherComp,
         			   int32 OtherBodyIndex);

	UFUNCTION()
	void OnWeaponBeginOverlap(class UPrimitiveComponent* OverlappedComp,
	                    class AActor* OtherActor,
	                    class UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex,
	                    bool bFromSweep,
	                    const FHitResult& SweepResult);

	UFUNCTION()
    void OnWeaponEndOverlap(UPrimitiveComponent* OverlappedComp,
                        AActor* OtherActor,
                        UPrimitiveComponent* OtherComp,
                    int32 OtherBodyIndex);
};