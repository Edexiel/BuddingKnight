// Fill out your copyright notice in the Description page of Project Settings.

#include "Pot.h"
#include "BaseComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

#include "Plant.h"
#include "PlayerCharacter.h"
#include "Spore.h"
#include "Engine/Engine.h"


// Sets default values
APot::APot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->InitSphereRadius(50.f);
	RootComponent = CollisionSphere;
	CollisionSphere->SetCanEverAffectNavigation(false);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(RootComponent);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	BaseComponent = CreateDefaultSubobject<UBaseComponent>(TEXT("BaseComponent"));
	
	SpawnPlantPoint = CreateDefaultSubobject<UBillboardComponent>(TEXT("SpawnPlantPoint"));
	SpawnPlantPoint->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APot::BeginPlay()
{
	Super::BeginPlay();
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &APot::OnBoxBeginOverlap);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &APot::OnBoxEndOverlap);
	
	Plant = nullptr;
	Player = nullptr;
	PlayerIsDetected = false;
	PassiveIsActive = false;
}

// Called every frame
void APot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(CanPlant && !HaveASeed)
	{
		switch(TypeOfPlant)
		{
			case EPlantType::Tree:
				Plant = GetWorld()->SpawnActor<APlant>(Tree, SpawnPlantPoint->GetComponentTransform());
				break;
			case EPlantType::Liana:
				Plant = GetWorld()->SpawnActor<APlant>(Liana, SpawnPlantPoint->GetComponentTransform());
				break;
			case EPlantType::Spore:
				Plant = GetWorld()->SpawnActor<APlant>(Spore, SpawnPlantPoint->GetComponentTransform());
				break;
			
			default:
				break;
		}
		
		CanPlant = false;
		HaveASeed = true;
		CollisionBox->UpdateOverlaps();
	}
	if(Plant && Plant->GetDetectPlayer() != PlayerIsDetected)
	{
		Plant->SetDetectPlayer(PlayerIsDetected);
		if(!PassiveIsActive)
		{
			Plant->Passive(Player);
			PassiveIsActive = true;
		}
	}
}

void APot::SetHaveASeed(const bool& Boolean)
{
	HaveASeed = Boolean;
}

bool APot::GetHaveASeed() const
{
	return HaveASeed;
}

void APot::SetCanPlant(const bool& Boolean)
{
	OnSeedPlanted(TypeOfPlant);
	CanPlant = Boolean;
}

void APot::SetTypeOfPlant(const TEnumAsByte<EPlantType> Type)
{
	TypeOfPlant = Type;
}

bool APot::GetCanPlant() const
{
	return CanPlant;
}

void APot::ReceiveDamage(const float Value) 
{
	BaseComponent->TakeDamage(Value);
	OnReceiveDamage(BaseComponent->GetHealth());

	if(BaseComponent->GetHealth()<=0.1f)
		OnPotDestroy();
}


void APot::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                             int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		PlayerIsDetected = true;
		PassiveIsActive = false;
		Player = Cast<APlayerCharacter>(OtherActor);
	}
}

void APot::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherActor->IsA(APlayerCharacter::StaticClass()) && Player == OtherActor)
	{
		if(IsValid(Plant))
		{
			if(Plant->IsA(ASpore::StaticClass()))
				Cast<ASpore>(Plant)->DestroyPassiveSpore();
		}
		PlayerIsDetected = false;
		PassiveIsActive = false;
		Player->UnsetBonusDamage();
		Player->ChangeSword(ESwords::E_Basic);
		Player = nullptr;
	}
}