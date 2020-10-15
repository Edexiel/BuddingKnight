// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool HaveASeed;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void SetHaveASeed(const bool& Boolean);
	bool GetHaveASeed() const;
};
