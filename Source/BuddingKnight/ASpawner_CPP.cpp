


#include "ASpawner_CPP.h"

#include "AIC_EnemyCAC.h"

AASpawner_CPP::AASpawner_CPP(){}

void AASpawner_CPP::BeginPlay()
{
	Super::BeginPlay();

	OnAfterBeginPlay();
}

AEnemy* AASpawner_CPP::SpawnEnemy(const TSubclassOf<AEnemy> Type)
{
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AEnemy* Enemy = GetWorld()->SpawnActor<AEnemy>(Type,GetTransform(),Params);
	
	if(!Enemy)
		return nullptr;
	
	AAIC_EnemyCAC* Aic = Cast<AAIC_EnemyCAC>(Enemy->GetController());
	Aic->SetTarget(Cast<AActor>(Target));
	
	OnEnemySpawn();

	return Enemy;
}

void AASpawner_CPP::LightBeacon()
{
	OnLightBeacon();
}

