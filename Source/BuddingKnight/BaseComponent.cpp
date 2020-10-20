// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseComponent.h"
#include "BaseComponentDataAsset.h"

// Sets default values for this component's properties
UBaseComponent::UBaseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBaseComponent::BeginPlay()
{
	Super::BeginPlay();
    
    if(BaseComponentDataAsset != nullptr)
    {
    	Health = BaseComponentDataAsset->GetHealth();
    	MaxHealth = BaseComponentDataAsset->GetMaxHealth();
    }
}


// Called every frame
void UBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

float UBaseComponent::GetHealth() const
{
	return Health;
}

float UBaseComponent::GetMaxHealth() const
{
	return MaxHealth;
}

void UBaseComponent::TakeDamage(const float Damage)
{
		Health - Damage <= 0.f? Health = 0.f : Health -= Damage;
}

