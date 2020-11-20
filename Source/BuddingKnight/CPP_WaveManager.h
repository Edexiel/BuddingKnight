

#pragma once

#include "CoreMinimal.h"


#include "CPP_WaveData.h"
#include "Enemy.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"
#include "CPP_WaveManager.generated.h"

UCLASS()
class BUDDINGKNIGHT_API ACPP_WaveManager : public AActor
{
	GENERATED_BODY()

	UPROPERTY()
	int Wave{0};

	UPROPERTY()
	int MaxWave{0};
	
	UPROPERTY()
	float DelayBetweenSpawn;
	
	UPROPERTY()
	float DelayBetweenWaves;
	
	UPROPERTY()
	float LightBeforeSpawn;

	UPROPERTY()
	float StartDelay;
	
	UPROPERTY()
	float Buffer{0};

	UPROPERTY()
	TArray<AEnemy*> Enemies;

	UPROPERTY()
	TArray<FTimerHandle> Timers;

	// UPROPERTY()
	// TArray<FTimerDelegate> TimerDelegates;

	UPROPERTY()
	bool bIsFinished{false};

	UPROPERTY()
	FTimerHandle StartWaveHandle;

	UFUNCTION()
	void WaveStart();

	UFUNCTION()
	void SpawnEnemy(AASpawner_CPP* Spawner, const TSubclassOf<AEnemy> Type);

	UFUNCTION()
	void OnEnemyDestroy(AActor* Enemy);
	
public:	
	ACPP_WaveManager();

	UPROPERTY(EditAnywhere)
	UCPP_WaveData* WaveAsset;

	UFUNCTION(BlueprintImplementableEvent)
	void OnGameEnd();

	UFUNCTION(BlueprintImplementableEvent)
	void OnWaveInit(FTimerHandle& Handle);

	UFUNCTION(BlueprintImplementableEvent)
	void OnWaveStart(int WaveNumber);

	UFUNCTION(BlueprintImplementableEvent)
	void OnWaveFinish(int NewWaveNumber);

	UFUNCTION(BlueprintCallable)
	int MaxWaves() const;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

};
