// Fill out your copyright notice in the Description page of Project Settings.


#include "Plant.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "PlayerCharacter.h"
#include "Enemy.h"


APlant::APlant()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereDetection = CreateDefaultSubobject<USphereComponent>(TEXT("SphereDetection"));
	SphereDetection->InitSphereRadius(500.f);
	SphereDetection->SetupAttachment(RootComponent);
	SphereDetection->SetCanEverAffectNavigation(false);
}

void APlant::BeginPlay()
{
	Super::BeginPlay();
	SphereDetection->OnComponentBeginOverlap.AddDynamic(this, &APlant::OnSphereDetectionOverlapBegin);
	SphereDetection->OnComponentEndOverlap.AddDynamic(this, &APlant::OnSphereDetectionOverlapEnd);

	CanUseSpecial = true;
	IsResettingDelay = true;
	ClosestEnemy = nullptr;

	RotSpeed = 2.5f;
}

void APlant::Delay()
{
	if(IsResettingDelay)
	{
		GetWorldTimerManager().ClearTimer(TimeHandleDelay);
		GetWorldTimerManager().SetTimer(TimeHandleDelay, this, &APlant::ResetDelay, DelayCooldown, false);
		IsResettingDelay = false;
	}
}

void APlant::ResetDelay()
{
	CanUseSpecial = true;
	IsResettingDelay = true;
}

void APlant::UseSpecial()
{
	if(!DetectPlayer)
		return;

	if(CanUseSpecial && DetectPlayer)
	{
		AnimationCooldown = PlayAnimMontage(AttackAnimation);
		Special();
		CanUseSpecial = false;
		Delay();
	}
}

void APlant::Special()
{
}

void APlant::SearchClosestEnemy()
{	
	if(Enemies.Num() == 0)
		return;
		
	if (Enemies.Num() == 1 && ClosestEnemy == nullptr)
	{
		ClosestEnemy = Enemies[0];
		return;
	}
	if (Enemies.Num() > 1 && ClosestEnemy == nullptr)
		ClosestEnemy = Enemies[0];
	
	for (AEnemy* Pawn : Enemies)
	{
		const float NewDistance = GetDistanceTo(Pawn);
		
		if (ClosestEnemy == Pawn)
		{
			DistanceToClosestEnemy = NewDistance;
		}
		
		else if(NewDistance < DistanceToClosestEnemy)
		{
			ClosestEnemy = Pawn;
			DistanceToClosestEnemy = NewDistance;
		}
	}
}

void APlant::LookAtClosestEnemy(const float DeltaTime)
{
	if(ClosestEnemy == nullptr)
		return;
	
	FRotator PlantRotation = GetActorRotation();
	const FRotator NewRotation =  UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ClosestEnemy->GetActorLocation());

	PlantRotation.SetComponentForAxis(EAxis::Z, NewRotation.Yaw);
	PlantRotation = GetActorRotation() + (PlantRotation - GetActorRotation()).GetNormalized() * DeltaTime * RotSpeed;

	SetActorRotation(PlantRotation);
}

void APlant::Tick(float DeltaTime)
{	
	Super::Tick(DeltaTime);
	
	SearchClosestEnemy();
	LookAtClosestEnemy(DeltaTime);
}

void APlant::SetDetectPlayer(const bool Boolean)
{
	DetectPlayer = Boolean;
}

bool APlant::GetDetectPlayer() const
{
	return DetectPlayer;
}

void APlant::Passive(APlayerCharacter* Player)
{
}

void APlant::OnSphereDetectionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->IsA(AEnemy::StaticClass()))
	{
	    Enemies.Add(Cast<AEnemy>(OtherActor));
	}
}

void APlant::OnSphereDetectionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor->IsA(AEnemy::StaticClass()))
	{
		Enemies.Remove(Cast<AEnemy>(OtherActor));
		if(OtherActor == ClosestEnemy)
		{
			ClosestEnemy = nullptr;
		}
	}
}