// Fill out your copyright notice in the Description page of Project Settings.


#include "Spore.h"
#include "PassiveSpore.h"
#include "PlayerCharacter.h"
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
   if(!Player)
      return;

   Player->ChangeSword(ESwords::E_Spore);
   PassiveSpore = GetWorld()->SpawnActor<APassiveSpore>(PassiveSporeClass, SpawnPointProjectile->GetComponentTransform());
}

void ASpore::DestroyPassiveSpore()
{
   if(PassiveSpore)
      PassiveSpore->Destroy();
}

void ASpore::Special()
{
   SpawnProjectile();
}

void ASpore::SpawnProjectile() const
{  
   ASporeProjectile* Projectile = GetWorld()->SpawnActor<ASporeProjectile>(ProjectileClass, SpawnPointProjectile->GetComponentTransform());

   if(Projectile)
      Projectile->SetTarget(ClosestEnemy);
}