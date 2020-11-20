#include "SporeProjectile.h"


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASporeProjectile::ASporeProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->InitSphereRadius(50.f);
	RootComponent = CollisionSphere;
	CollisionSphere->SetCanEverAffectNavigation(false);
}

// Called when the game starts or when spawned
void ASporeProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ASporeProjectile::OnSphereBeginOverlap);
}

void ASporeProjectile::OnSphereBeginOverlap(class UPrimitiveComponent* OverlappedComp,
											class AActor* OtherActor,
											class UPrimitiveComponent* OtherComp,
											int32 OtherBodyIndex, bool bFromSweep,
											const FHitResult& SweepResult)
{
	if(OtherActor->IsA(AEnemy::StaticClass()))
	{
		OnImpact();
		Cast<AEnemy>(OtherActor)->OnDamageReceiveByTick(Damage, NbTick);
	}
	Destroy();
}

// Called every frame
void ASporeProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	ChaseTarget(DeltaTime);
}

void ASporeProjectile::ChaseTarget(float DeltaTime)
{
	const FVector ProjectileLocation = GetActorLocation();
	
	if(!Target)
	{
		SetActorLocation(ProjectileLocation + GetActorForwardVector() * DeltaTime * Speed);
		return;
	}
	
	const FVector TargetLocation = Target->GetActorLocation();
	
	const FVector Dir = (TargetLocation - ProjectileLocation).GetSafeNormal();
	SetActorLocation(ProjectileLocation + Dir * DeltaTime * Speed);
}

void ASporeProjectile::SetTarget(AActor* NewTarget)
{
	Target = NewTarget;
}

