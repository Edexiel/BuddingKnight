

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Zone.generated.h"

UCLASS()
class BUDDINGKNIGHT_API AZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AZone();

	UPROPERTY(EditAnywhere, Category=Pot)
	class APot * Pot1;
	
	UPROPERTY(EditAnywhere, Category=Pot)
	class APot * Pot2;
	
	UPROPERTY(EditAnywhere, Category=Pot)
	class APot * Pot3;
	
protected:
	// Called when the game starts or when spawned
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* CollisionBox;
	
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
