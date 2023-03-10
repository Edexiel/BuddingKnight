// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include <string>



#include "AIC_EnemyCAC.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "PlayerCharacter.h"
#include "Pot.h"
#include "TimerManager.h"
#include "Tree.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"

// Sets default values
AEnemy::AEnemy()
{
	BaseComponent = CreateDefaultSubobject<UBaseComponent>(TEXT("BaseComponent"));
	
	RightWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightWeapon"));
	RightWeapon->SetupAttachment(GetMesh(),TEXT("RightWeaponShield"));
	RightWeapon->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnWeaponBeginOverlap);
	RightWeapon->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnWeaponEndOverlap);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	IsAlreadyTakeDamage = false;
	RightWeapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	IsResettingTickDamageDelay = true;
	OnDamageReceiveByTickCooldown = 1.f;
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AEnemy::IsDead() const
{
	return BaseComponent->IsDead();
}

void AEnemy::ReceiveDamage(const float Value) 
{
	if(IsGettingHit || IsDead())
		return;

	OnDamageReceive();
	
	IsGettingHit = true;

	if(!bIsFrozen)
	{
		LaunchCharacter(GetActorForwardVector() * KnockBackForce * -1,true,true);
	}
	
	const float RemainingTime = PlayAnimMontage(GettingHitAnimMontage);
	GetWorldTimerManager().SetTimer(GettingHitHandle,this,&AEnemy::ResetGettingHit,RemainingTime,false);

	BaseComponent->TakeDamage(Value);

	if(IsDead())
	{
		OnDeath(GetActorLocation());
		GetCapsuleComponent()->DestroyComponent();
		RightWeapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Cast<AAIC_EnemyCAC>(GetController())->SetDead();
		GetWorldTimerManager().SetTimer(DestroyHandle,this,&AEnemy::Delete,DepopTime,false);
	}
}


void AEnemy::OnDamageReceiveByTick(const float Value, int NbOfTick)
{
	if(IsDead() || NbOfTick < 1)
		return;

	//GEngine->AddOnScreenDebugMessage(NULL,2.f,FColor::Green,TEXT("Tick = " + FString::FromInt( NbOfTick)));
	DamageReceive = Value;
	NbTick = NbOfTick;

	
	
	BaseComponent->TakeDamage(DamageReceive);
	
	TickDamageDelay();
	
	if(IsDead())
	{
		OnDeath(GetActorLocation());
		GetCapsuleComponent()->DestroyComponent();
		RightWeapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Cast<AAIC_EnemyCAC>(GetController())->SetDead();
		GetWorldTimerManager().SetTimer(DestroyHandle,this,&AEnemy::Delete,DepopTime,false);
	}
}

void AEnemy::WeaponCollisionTest() const
{
	RightWeapon->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::WeaponCollisionTestEnd()
{
	RightWeapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bTouchedPlayer=false;
}

void AEnemy::Attack()
{
	if(!IsDead())
		PlayAnimMontage(AttackAnimMontage);
}

void AEnemy::Freeze()
{
	if(!IsValid(GetController()))
		return;
	
	bIsFrozen=true;
	
	OnLianaFreeze();
	GetController()->StopMovement(); 
	Cast<AAIC_EnemyCAC>(GetController())->GetBrainComponent()->PauseLogic("Freezed");
}

void AEnemy::UnFreeze()
{
	if(!IsValid(GetController()))
		return;

	bIsFrozen=false;
	
	OnLianaUnFreeze();
	
	Cast<AAIC_EnemyCAC>(GetController())->GetBrainComponent()->ResumeLogic("UnFreeze");
}

void AEnemy::ResetGettingHit()
{
	IsGettingHit=false;
}

void AEnemy::Delete()
{
	Destroy();
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::OnWeaponBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		OnPlayerHit();
		Cast<APlayerCharacter>(OtherActor)->ReceiveDamage();
		bTouchedPlayer=true;
	}

	if(OtherActor->IsA(APot::StaticClass()))
	{
		Cast<APot>(OtherActor)->ReceiveDamage(Damage);
		OnPotHit();
	}
	
}

void AEnemy::OnWeaponEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	bTouchedPlayer=false;
	bTouchedPot=false;
}

void AEnemy::TickDamageDelay()
{
	if(IsResettingTickDamageDelay)
	{
		GetWorldTimerManager().ClearTimer(TickDamageTimeHandle);
		GetWorldTimerManager().SetTimer(TickDamageTimeHandle, this, &AEnemy::ResetTickDamageDelay, OnDamageReceiveByTickCooldown, false);
		IsResettingTickDamageDelay = false;
	}
}

void AEnemy::ResetTickDamageDelay()
{
	IsResettingTickDamageDelay = true;
	NbTick--;
	OnDamageReceiveByTick(DamageReceive, NbTick);
}
