

#pragma once

#include "CoreMinimal.h"

#include "ASpawner_CPP.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"

#include "CPP_WaveData.generated.h"

USTRUCT(BlueprintType)
struct FSpawnerInfos : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Time;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Number;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AEnemy> Type;
};

USTRUCT(BlueprintType)
struct FWaveInfos
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<AASpawner_CPP> Spawner;
	
	UPROPERTY(EditAnywhere)
	UDataTable* Scenario;
};

USTRUCT(BlueprintType)
struct FWave
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FWaveInfos> WaveInfos;  
};

UCLASS(BlueprintType)
class BUDDINGKNIGHT_API UCPP_WaveData : public UDataAsset
{
	GENERATED_BODY()

	public:
	
	UPROPERTY(EditAnywhere)
	float DelayBetweenSpawn;

	UPROPERTY(EditAnywhere)
	float DelayBetweenWaves;

	UPROPERTY(EditAnywhere)
	float LightBeforeSpawn;

	UPROPERTY(EditAnywhere)
	float StartDelay;

	UPROPERTY(EditAnywhere)
	TArray<FWave> Wave;
};
