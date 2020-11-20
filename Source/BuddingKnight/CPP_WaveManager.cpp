


#include "CPP_WaveManager.h"

#include "TimerManager.h"


// Sets default values
ACPP_WaveManager::ACPP_WaveManager()
{
}

// Called when the game starts or when spawned
void ACPP_WaveManager::BeginPlay()
{
	Super::BeginPlay();
	DelayBetweenSpawn = WaveAsset->DelayBetweenSpawn ;
	DelayBetweenWaves = WaveAsset->DelayBetweenWaves;
	LightBeforeSpawn = WaveAsset->LightBeforeSpawn;
	StartDelay = WaveAsset->StartDelay;

	MaxWave= WaveAsset->Wave.Num();

	FTimerHandle DelayStart;
	GetWorldTimerManager().SetTimer(DelayStart,this,&ACPP_WaveManager::WaveStart,StartDelay,false);
	OnGameStart(DelayStart);
}

void ACPP_WaveManager::WaveStart()
{
	if(bIsFinished)
	{
		return;
	}
	OnWaveStart(Wave);

	for (FWaveInfos& WaveInfos : WaveAsset->Wave[Wave].WaveInfos)
	{
		// WaveInfos.Spawner;
		for (auto Map: WaveInfos.Scenario->GetRowMap())
		{
			FSpawnerInfos* Scenario = reinterpret_cast<FSpawnerInfos*>(Map.Value);

			FTimerHandle Td;
			const float Time = (Scenario->Time - LightBeforeSpawn)<=0?0.1f:Scenario->Time - LightBeforeSpawn;
			GetWorldTimerManager().SetTimer(Td,WaveInfos.Spawner.Get(),&AASpawner_CPP::LightBeacon,Time,false);

			for (int i = 0; i < Scenario->Number; ++i)
			{
				
				FTimerHandle Timer;// = Timers.Emplace_GetRef();
				FTimerDelegate TimerDelegate;
				
				TimerDelegate.BindUFunction(this,FName("SpawnEnemy"),WaveInfos.Spawner.Get(),Scenario->Type);

				GetWorldTimerManager().SetTimer(Timer,TimerDelegate,Scenario->Time+Buffer,false);

				Buffer+=DelayBetweenSpawn;
			}
			Buffer = 0;
		}		
	}
}

void ACPP_WaveManager::SpawnEnemy(AASpawner_CPP* Spawner, const TSubclassOf<AEnemy> Type)
{
	
	AEnemy* Enemy = Spawner->SpawnEnemy(Type);

	Enemies.Add(Enemy);
	Enemy->OnDestroyed.AddDynamic(this,&ACPP_WaveManager::OnEnemyDestroy);
	
	// Timer.Invalidate();
	// Timers.Remove(Timer);

}

void ACPP_WaveManager::OnEnemyDestroy(AActor* Enemy)
{
	Enemies.Remove(Cast<AEnemy>(Enemy));

	if(Enemies.Num()==0)
	{
		Wave++;
		bIsFinished=Wave==MaxWave;

		if(bIsFinished)
		{
			OnGameEnd();
		}
		else
		{
			GetWorldTimerManager().SetTimer(StartWaveHandle,this,&ACPP_WaveManager::WaveStart,DelayBetweenWaves,false);
			OnWaveFinish(Wave);
		}	
	}
}
