#include "PassiveSpore.h"



#include "Enemy.h"
#include "Pot.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

// Sets default values
APassiveSpore::APassiveSpore()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->InitSphereRadius(50.f);
	RootComponent = CollisionSphere;

}

// Called when the game starts or when spawned
void APassiveSpore::BeginPlay()
{
	Super::BeginPlay();
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &APassiveSpore::OnSphereComponentBeginOverlap);
}

void APassiveSpore::OnSphereComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->IsA(AEnemy::StaticClass()))
	{
		Cast<AEnemy>(OtherActor)->ReceiveDamage(Damage);
	}
}

// Called every frame
void APassiveSpore::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// rotate around player
	FVector NewLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	
	AngleAxis += DeltaTime * Speed;

	if(AngleAxis >= 360.0f) 
	{
		AngleAxis = 0;
	}

	FVector RotateValue = DistanceToPlayer.RotateAngleAxis(AngleAxis, GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation().UpVector);

	NewLocation.X += RotateValue.X;
	NewLocation.Y += RotateValue.Y;
	NewLocation.Z += RotateValue.Z;

	FRotator NewRotation = FRotator(0, AngleAxis, 0);
	
	FQuat QuatRotation = FQuat(NewRotation);

	SetActorLocationAndRotation(NewLocation, QuatRotation, false, 0, ETeleportType::None);
}

