// Fill out your copyright notice in the Description page of Project Settings.


#include "Plant.h"
#include "Components/SphereComponent.h"

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
	
}

// Called every frame
void APlant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APlant::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

