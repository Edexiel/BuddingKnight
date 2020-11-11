// Fill out your copyright notice in the Description page of Project Settings.


#include "Spore.h"

#include "Enemy.h"
#include "SporeProjectile.h"
#include "Engine/World.h"

ASpore::ASpore()
{
   SpawnPointProjectile = CreateDefaultSubobject<UChildActorComponent>(TEXT("SpawnProjectile"));
   SpawnPointProjectile->SetupAttachment(RootComponent);
}

void ASpore::Passive(APlayerCharacter* Player)
{
   if(Player)
      return;
}

void ASpore::Special()
{
   SpawnProjectile();
}

void ASpore::SpawnProjectile() const
{
   /*FActorSpawnParameters Param;
   Param.bAllowDuringConstructionScript = false;
   Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;*/
   
   ASporeProjectile* Projectile = GetWorld()->SpawnActor<ASporeProjectile>(ProjectileClass, SpawnPointProjectile->GetComponentTransform());

   if(Projectile)
      Projectile->SetTarget(ClosestEnemy);
}