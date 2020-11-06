// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Seed.h"
#include "Components/BillboardComponent.h"
#include "Components/ChildActorComponent.h"
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
		
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool HaveASeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool CanPlant;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EPlantType> TypeOfPlant;

	// the actual plant
	UPROPERTY(VisibleAnywhere,Category="Plant", BlueprintReadWrite)
	class APlant* Plant;

	

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
};