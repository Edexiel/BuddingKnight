// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Seed.generated.h"

UENUM(BlueprintType)
enum EPlantType
{
	Tree,
	Liana,
	Spore,
	NbType
};

UCLASS()
class BUDDINGKNIGHT_API ASeed : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASeed();
	
	UPROPERTY(EditAnywhere)
	class USphereComponent* CollisionSphere;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Mesh;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EPlantType> Type;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	TEnumAsByte<EPlantType> GetType() const;
};
