// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Seed.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UENUM(Blueprintable)
enum ESwords
{
	E_Basic,
	E_Plant,
	E_Tree,
	E_Spore
};

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* RightWeapon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TMap<TEnumAsByte<ESwords>,UStaticMesh*> Swords;
	
	/** Attack Animation **/
	UPROPERTY(EditAnywhere,Category=Animation,meta=(AllowPrivateAccess="true"))
	TArray<class UAnimMontage*> Combo;	

	/** Stun Animation **/
	UPROPERTY(EditAnywhere,Category=Animation,meta=(AllowPrivateAccess="true"))
	class UAnimMontage* StunAnimation;

	/** Stun Hit Animation **/
	UPROPERTY(EditAnywhere,Category=Animation,meta=(AllowPrivateAccess="true"))
	class UAnimMontage* GetHitAnimation;


	UPROPERTY(EditAnywhere, Category=Camera,meta=(AllowPrivateAccess="true"))
	class UCameraDataAsset* DataAssetCamera;
	
	/** Reference to MovementComponent **/
	class UPawnMovementComponent*  MovementComponent;

	/**	Attack counter for combo **/
	uint32 AttackCounter{0};
	
	/** Duration of stun **/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess="true"))
	float StunTime;

	/** Slow down rate [0-1] **/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess="true"))
	float SlowDownRate;

	/** Time without hit when the player recovers his speed **/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess="true"))
	float SlowDownTime;

	/** Fighting Slow down rate [0-1] **/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess="true"))
	float InFightSpeedRate;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="MovementSpeed",meta=(AllowPrivateAccess="true"))
	float WalkSpeed{600};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="MovementSpeed",meta=(AllowPrivateAccess="true"))
	float RollSpeed{10000};

	UPROPERTY()
	bool bRollOnce{false};
	
	int HitReceivedCounter{0};
	
	//BPStates
	//CanAttack, used for combo
	UPROPERTY(BlueprintReadWrite,meta=(AllowPrivateAccess="true"))
	bool bCanAttack{true};
	
	UPROPERTY(BlueprintReadWrite,meta=(AllowPrivateAccess="true"))
	bool bIsRolling{false};

	bool bIsStun{false};

	UPROPERTY(EditAnywhere)
	float BaseDamage;

	UPROPERTY()
	float BonusDamage;

	FTimerHandle StunHandle;
	FTimerHandle SlowDownHandle;
	
	bool bTouchedEnemy;
	
	float SlowDownAccumulator{0};

	UFUNCTION()
	void ResetCanAttack();

	UFUNCTION()
	void ResetSlowDown();

	UFUNCTION()
	void ResetStun();


public:
	APlayerCharacter();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Zone)
	class AZone* Zone;
	
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InvertControl")
	bool IsInvertPitchAxis{false};
	
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

	UFUNCTION(BlueprintCallable)
    void WeaponCollisionTest() const;
	
	UFUNCTION(BlueprintCallable)
	void WeaponCollisionTestEnd();

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
	
	//Rotate the CameraBoom in the direction of LockEnemy.
	UFUNCTION()
    void CameraLock(const float DeltaTime);

	//Search the closest enemy in enemies.
	UFUNCTION()
    void SearchClosestEnemy();

	UFUNCTION(BlueprintCallable)
    void TakeSeed(ASeed* Seed);
	
	UFUNCTION(BlueprintCallable)
    void UseSeed();

	UFUNCTION(BlueprintCallable)
	void ReceiveDamage();

	UFUNCTION(BlueprintCallable)
	void ChangeSword(ESwords Sword);

	UFUNCTION(BlueprintCallable)
	bool IsStun() const;
	
	/* Events */
	
	UFUNCTION(BlueprintImplementableEvent)
    void OnDamageReceive();

	UFUNCTION(BlueprintImplementableEvent)
	void OnStun();

	UFUNCTION(BlueprintImplementableEvent)
	void OnResetCombo();

	UFUNCTION(BlueprintImplementableEvent)
	void OnCombo(int Value);

	UFUNCTION(BlueprintImplementableEvent)
	void OnRecoverSpeed();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnDamageBonus();

	UFUNCTION(BlueprintImplementableEvent)
	void OnRecoverStun();

	UFUNCTION(BlueprintImplementableEvent)
	void OnChangeSword(ESwords SwordType);

	UFUNCTION(BlueprintImplementableEvent)
	void OnEnemyHit();

	UFUNCTION(BlueprintImplementableEvent)
	void OnSeedPick(EPlantType Type);

	/* End events */

	/* Collisions */
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

	/* End Collisions */

	virtual void AddControllerPitchInput(float Val) override;
	
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
	void Dodge();
	void SelectLeft();
	void SelectRight();

	UFUNCTION(BlueprintCallable)
	void OnValidateAttack();
	
	UFUNCTION(BlueprintCallable)
	void ResetCombo();

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

};