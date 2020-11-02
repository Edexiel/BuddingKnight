// Fill out your copyright notice in the Description page of Project Settings.


#include "Plant.h"

#include <activation.h>

#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

#include "PlayerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
APlant::APlant()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereDetection = CreateDefaultSubobject<USphereComponent>(TEXT("SphereDetection"));
	SphereDetection->InitSphereRadius(500.f);
	SphereDetection->SetupAttachment(RootComponent);
	SphereDetection->SetCanEverAffectNavigation(false);
}

// Called when the game starts or when spawned
void APlant::BeginPlay()
{
	Super::BeginPlay();
	SphereDetection->OnComponentBeginOverlap.AddDynamic(this, &APlant::OnSphereDetectionOverlapBegin);
	SphereDetection->OnComponentEndOverlap.AddDynamic(this, &APlant::OnSphereDetectionOverlapEnd);

	CanUseSpecial = true;
	IsResettingDelay = true;
	DelayCooldown = 20.f;
	
	ClosestEnemy = nullptr;
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
	
	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this,0));

	if(CanUseSpecial && DetectPlayer)
	{
		GEngine->AddOnScreenDebugMessage(NULL,2.f,FColor::Cyan,"Use special");
		CanUseSpecial = false;
		Delay();
	}
}

void APlant::SearchClosestEnemy()
{
	if(Enemies.Num() == 0)
		return;
	
	else if (Enemies.Num() > 1 && ClosestEnemy == nullptr)
		ClosestEnemy = Enemies[0];
	
	for (APawn* Pawn : Enemies)
	{
		const float NewDistance = GetDistanceTo(Pawn);
		
		//UE_LOG(LogTemp, Warning, TEXT("DistanceToClosestEnemy = %f"), DistanceToClosestEnemy);
		//UE_LOG(LogTemp, Warning, TEXT("NewDistance = %f"), NewDistance);
		
		if (ClosestEnemy == Pawn)
		{
			DistanceToClosestEnemy = NewDistance;
		}
		
		if(NewDistance < DistanceToClosestEnemy)
		{
			ClosestEnemy = Pawn;
			DistanceToClosestEnemy = NewDistance;
			UE_LOG(LogTemp, Warning, TEXT("Change closest enemy"));
		}
	}
	SetFocus(ClosestEnemy);
}

// Called every frame
void APlant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SearchClosestEnemy();
}

void APlant::OnSphereDetectionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		DetectPlayer = true;
		return;
	}
	
	if(OtherActor->IsA(APawn::StaticClass()))
	{
	    Enemies.Add(Cast<APawn>(OtherActor));
		GEngine->AddOnScreenDebugMessage(NULL,2.f,FColor::Green,"Add enemy!");
	}
}

void APlant::OnSphereDetectionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		DetectPlayer = false;
		return;
	}

	if(OtherActor->IsA(APawn::StaticClass()))
	{
		Enemies.Remove(Cast<APawn>(OtherActor));
		GEngine->AddOnScreenDebugMessage(NULL,2.f,FColor::Red,"Remove enemy!");
	}
}