// Fill out your copyright notice in the Description page of Project Settings.


#include "Tree.h"
#include "Components/BoxComponent.h"

ATree::ATree()
{
    AttackBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackBox"));
    AttackBox->SetBoxExtent({100,100,100});
    AttackBox->SetupAttachment(RootComponent);
    AttackBox->SetCanEverAffectNavigation(false);
}

void ATree::BeginPlay()
{
    Super::BeginPlay();
}

void ATree::Special()
{
}
