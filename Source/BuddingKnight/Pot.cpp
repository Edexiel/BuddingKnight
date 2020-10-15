// Fill out your copyright notice in the Description page of Project Settings.


#include "Pot.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
APot::APot()
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
void APot::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APot::SetHaveASeed(const bool& Boolean)
{
	HaveASeed = Boolean;
}

bool APot::GetHaveASeed() const
{
	return HaveASeed;
}

