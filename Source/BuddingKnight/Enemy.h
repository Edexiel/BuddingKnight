// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "BaseComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class BUDDINGKNIGHT_API AEnemy : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UBaseComponent* BaseComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* AttackAnimMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* GettingHitAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Damage{0};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float KnockBackForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))

	float DepopTime;
	
	void ResetGettingHit();
	void Delete();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY()
	bool IsGettingHit {false};

	UPROPERTY()
	FTimerHandle GettingHitHandle;
	
	UPROPERTY()
	FTimerHandle DestroyHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* RightWeapon;

	UPROPERTY()
	bool bTouchedPlayer{false};

	UPROPERTY()
	bool bTouchedPot{false};
	
public:

	AEnemy();
	
	UPROPERTY()
	bool IsAlreadyTakeDamage;
	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	bool IsDead() const;

	UFUNCTION(BlueprintCallable)
	void ReceiveDamage(const float Value);

	UFUNCTION(BlueprintImplementableEvent)
	void OnDamageReceive();

	UFUNCTION(BlueprintImplementableEvent)
    void OnDeath(FVector Position);

	UFUNCTION(BlueprintCallable)
    void ReceiveDamageByTick(const float Damage);

	UFUNCTION(BlueprintCallable)
	void WeaponCollisionTest() const;

	UFUNCTION(BlueprintCallable)
	void WeaponCollisionTestEnd();

	UFUNCTION()
	void Attack();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
    void OnWeaponBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                               int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
    void OnWeaponEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                             int32 OtherBodyIndex);
};
