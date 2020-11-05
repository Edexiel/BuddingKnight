// Fill out your copyright notice in the Description page of Project Settings.


#include "Seed.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ASeed::ASeed()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->InitSphereRadius(50.f);
	RootComponent = CollisionSphere;
	CollisionSphere->SetCanEverAffectNavigation(false);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	
}

// Called when the game starts or when spawned
void ASeed::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASeed::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

TEnumAsByte<EPlantType> ASeed::GetType() const
{
	return Type;
}
