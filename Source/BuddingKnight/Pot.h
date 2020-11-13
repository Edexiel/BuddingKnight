// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Seed.h"
#include "Components/BillboardComponent.h"
#include "GameFramework/Actor.h"
#include "Pot.generated.h"

UCLASS()
class BUDDINGKNIGHT_API APot : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APot();

	UPROPERTY(EditAnywhere)
	class USphereComponent* CollisionSphere;
	
	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UBaseComponent* BaseComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UBillboardComponent * SpawnPlantPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TSubclassOf<class APlant> Tree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TSubclassOf<class APlant> Liana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TSubclassOf<class APlant> Spore;

	UFUNCTION(BlueprintImplementableEvent)
    void OnPlayerEnter();

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerLeave();
		
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool HaveASeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool CanPlant;

	UPROPERTY()
	bool PlayerIsDetected;
	
	UPROPERTY()
	bool PassiveIsActive;

	UPROPERTY(BlueprintReadWrite)
	bool IsDead;
	
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EPlantType> TypeOfPlant;

	// the actual plant
	UPROPERTY(VisibleAnywhere,Category="Plant", BlueprintReadWrite)
	class APlant* Plant;
	
	UPROPERTY()
	class APlayerCharacter* Player;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void SetHaveASeed(const bool& Boolean);
	UFUNCTION()
	bool GetHaveASeed() const;
	
	UFUNCTION()
	void SetCanPlant(const bool& Boolean);

	UFUNCTION()
    void SetTypeOfPlant(const TEnumAsByte<EPlantType> Type);
	
	UFUNCTION()
	bool GetCanPlant() const;

	UFUNCTION()
	void ReceiveDamage(const float Value);

	UFUNCTION(BlueprintImplementableEvent)
    void OnReceiveDamage(const float RemainingLife);

	UFUNCTION(BlueprintImplementableEvent)
    void OnPotDestroy();

	UFUNCTION(BlueprintImplementableEvent)
    void OnSeedPlanted(EPlantType Type);

protected:
	
	UFUNCTION()
    void OnBoxBeginOverlap(class UPrimitiveComponent* OverlappedComp,
                        class AActor* OtherActor,
                        class UPrimitiveComponent* OtherComp,
                        int32 OtherBodyIndex,
                        bool bFromSweep,
                        const FHitResult& SweepResult);

	UFUNCTION()
    void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp,
                        AActor* OtherActor,
                        UPrimitiveComponent* OtherComp,
                        int32 OtherBodyIndex);
};