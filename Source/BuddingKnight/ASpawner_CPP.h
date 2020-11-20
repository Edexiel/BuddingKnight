

#pragma once

#include "CoreMinimal.h"

#include "Enemy.h"
#include "Pot.h"
#include "GameFramework/Actor.h"
#include "ASpawner_CPP.generated.h"

UCLASS()
class BUDDINGKNIGHT_API AASpawner_CPP : public AActor
{
	GENERATED_BODY()
	
public:
	AASpawner_CPP();

	UPROPERTY(EditAnywhere)
	APot* Target;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnEnemySpawn();

	UFUNCTION(BlueprintImplementableEvent)
	void OnLightBeacon();

	UFUNCTION(BlueprintImplementableEvent)
	void OnAfterBeginPlay();

protected:
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable)
	AEnemy* SpawnEnemy(TSubclassOf<class AEnemy> Type);

	UFUNCTION(BlueprintCallable)
	void LightBeacon();

};
