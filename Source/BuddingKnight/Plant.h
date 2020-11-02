// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"

#include "Plant.generated.h"

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
	
	UPROPERTY()
	float DelayCooldown{5.f};

	UPROPERTY()
	TArray<APawn*> Enemies;

	UPROPERTY()
	APawn* ClosestEnemy;

	UPROPERTY()
	float DistanceToClosestEnemy{0.f};
	
	UPROPERTY()
	FTimerHandle TimeHandleDelay;
	
	UFUNCTION()
	void Delay();

	UFUNCTION()
    void ResetDelay();
	
	UFUNCTION(BlueprintCallable, meta=(AllowPrivateAccess = "true"))
	virtual void UseSpecial();

	UFUNCTION()
    void SearchClosestEnemy();

	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

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