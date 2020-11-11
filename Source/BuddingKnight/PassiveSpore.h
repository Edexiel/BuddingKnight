#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PassiveSpore.generated.h"

UCLASS()
class BUDDINGKNIGHT_API APassiveSpore : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APassiveSpore();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class USphereComponent* CollisionSphere;

	UPROPERTY(EditAnywhere)
	float Damage;
	
	// declare our float variables
	UPROPERTY(EditAnywhere, Category = Movement)
	float AngleAxis;

	UPROPERTY(EditAnywhere, Category = Movement)
	FVector DistanceToPlayer;
	

	UPROPERTY(EditAnywhere, Category = Movement)
	float Speed;

	UFUNCTION()
    void OnSphereComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp,
                        class AActor* OtherActor,
                        class UPrimitiveComponent* OtherComp,
                        int32 OtherBodyIndex,
                        bool bFromSweep,
                        const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;	
};
