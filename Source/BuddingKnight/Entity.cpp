// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity.h"
#include "GameFramework/Actor.h"

AEntity::AEntity()
{
    BaseComponent = CreateDefaultSubobject<UBaseComponent>(TEXT("BaseComponent"));
}
