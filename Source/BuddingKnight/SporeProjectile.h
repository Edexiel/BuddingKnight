#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SporeProjectile.generated.h"

UCLASS()
class BUDDINGKNIGHT_API ASporeProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASporeProjectile();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnImpact();

protected:
	UPROPERTY(EditAnywhere)
	class USphereComponent* CollisionSphere;

	UPROPERTY()
	AActor* Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage{0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NbTick{0};

	UPROPERTY(EditAnywhere)
	float Speed;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereBeginOverlap(class UPrimitiveComponent* OverlappedComp,
							  class AActor* OtherActor,
							  class UPrimitiveComponent* OtherComp,
							  int32 OtherBodyIndex, bool bFromSweep,
							  const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void ChaseTarget(float DeltaTime);

	UFUNCTION()
	void SetTarget(AActor* NewTarget = nullptr);

private:
	
};