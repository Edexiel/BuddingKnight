// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"

#include "Plant.generated.h"

class AEnemy;

UCLASS()
class BUDDINGKNIGHT_API APlant : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlant();
	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	class USphereComponent* SphereDetection;
	
	UPROPERTY()
	bool CanUseSpecial{true};

	UPROPERTY()
	bool DetectPlayer{false};

	UPROPERTY()
	bool IsResettingDelay{true};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DelayCooldown{5.f};

	UPROPERTY()
	float AnimationCooldown;

	UPROPERTY()
	TArray<AEnemy*> Enemies;

	UPROPERTY()
	AEnemy* ClosestEnemy;

	UPROPERTY()
	float DistanceToClosestEnemy{0.f};

	UPROPERTY()
	float RotSpeed;

	
	UPROPERTY()
	FTimerHandle TimeHandleDelay;

	UPROPERTY(EditAnywhere,Category=Animation,meta=(AllowPrivateAccess="true"))
	class UAnimMontage* SpawnAnimation;
	
	UPROPERTY(EditAnywhere,Category=Animation,meta=(AllowPrivateAccess="true"))
	class UAnimMontage* AttackAnimation;

	UFUNCTION()
	void Delay();

	UFUNCTION()
    void ResetDelay();
	
	UFUNCTION(BlueprintCallable, meta=(AllowPrivateAccess = "true"))
	void UseSpecial();

	virtual void Special();

	UFUNCTION()
    void SearchClosestEnemy();

	UFUNCTION()
    void LookAtClosestEnemy(const float DeltaTime);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetDetectPlayer(const bool Boolean);

	UFUNCTION(BlueprintCallable)
    bool GetDetectPlayer() const;
	
	// Called to bind functionality to input

	UFUNCTION()
    void OnSphereDetectionOverlapBegin(class UPrimitiveComponent* OverlappedComp,
                        class AActor* OtherActor,
                        class UPrimitiveComponent* OtherComp,
                        int32 OtherBodyIndex, bool bFromSweep,
                        const FHitResult& SweepResult);

	UFUNCTION()
    void OnSphereDetectionOverlapEnd(UPrimitiveComponent* OverlappedComp,
                        AActor* OtherActor,
                        UPrimitiveComponent* OtherComp,
                        int32 OtherBodyIndex);
};