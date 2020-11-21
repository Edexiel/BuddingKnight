// Fill out your copyright notice in the Description page of Project Settings.


#include "Seed.h"

#include "PlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstance.h"

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
	
	TreeMaterial = CreateDefaultSubobject<UMaterialInstance>(TEXT("TreeMaterial"));
	LianaMaterial = CreateDefaultSubobject<UMaterialInstance>(TEXT("LianaMaterial"));
	SporeMaterial = CreateDefaultSubobject<UMaterialInstance>(TEXT("SporeMaterial"));
}

// Called when the game starts or when spawned
void ASeed::BeginPlay()
{
	Super::BeginPlay();

	RandomTypeOfSeed();
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ASeed::OnCollisionSphereBeginOverlap);
	OnAfterBeginPlay();
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

void ASeed::RandomTypeOfSeed()
{
	const float Rate = FMath::RandRange(0.f, DropRateTree + DropRateLiana + DropRateSpore);
	
	if(Rate >= 0 && Rate < DropRateTree)
	{
		Type = EPlantType::Tree;
		if(TreeMaterial)
			Mesh->SetMaterial(0, TreeMaterial);
	}
	else if(Rate >= DropRateTree && Rate < DropRateTree + DropRateLiana)
	{
		Type = EPlantType::Liana;
		if(LianaMaterial)
			Mesh->SetMaterial(0, LianaMaterial);
	}
	else if(Rate >= DropRateTree + DropRateLiana && Rate < DropRateTree + DropRateLiana + DropRateSpore)
	{
		Type = EPlantType::Spore;
		if(SporeMaterial)
			Mesh->SetMaterial(0, SporeMaterial);
	}
}

void ASeed::OnCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		OnPickedByPlayer();
		Cast<APlayerCharacter>(OtherActor)->TakeSeed(this);
		Destroy();
	}
}
