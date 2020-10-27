// Fill out your copyright notice in the Description page of Project Settings.


#include "Plant.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"

#include "PlayerCharacter.h"

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
	
	
}

void APlant::UseSpecial()
{
	if(DetectPlayer && CanUseSpecial)
	{
		GEngine->AddOnScreenDebugMessage(NULL,2.f,FColor::Cyan,"Use special");
	}
	
	UE_LOG(LogTemp, Warning, TEXT("DetectPlayer = %d"), DetectPlayer);
	UE_LOG(LogTemp, Warning, TEXT("CanUseSpecial = %d"), CanUseSpecial);
	
}

// Called every frame
void APlant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UseSpecial();
}

void APlant::OnSphereDetectionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->IsA(APlayerCharacter::StaticClass()))
		DetectPlayer = true;

	//GEngine->AddOnScreenDebugMessage(NULL,2.f,FColor::Green,"DetectPlayer is true");
}

void APlant::OnSphereDetectionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor->IsA(APlayerCharacter::StaticClass()))
		DetectPlayer = false;

	//GEngine->AddOnScreenDebugMessage(NULL,2.f,FColor::Red,"DetectPlayer is false");
}