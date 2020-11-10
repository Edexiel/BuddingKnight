// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"


#include "AIC_EnemyCAC.h"
#include "Components/CapsuleComponent.h"
#include "PlayerCharacter.h"
#include "Pot.h"
#include "TimerManager.h"
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
	
	RightWeapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
		
	LaunchCharacter(GetActorForwardVector() * KnockBackForce * -1,true,true);
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
	PlayAnimMontage(AttackAnimMontage);
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
		//GEngine->AddOnScreenDebugMessage(INDEX_NONE,5.f,FColor::Red,"Enemy touching "+OtherActor->GetActorLabel());
		Cast<APlayerCharacter>(OtherActor)->ReceiveDamage();
		bTouchedPlayer=true;
	}

	if(OtherActor->IsA(APot::StaticClass()))
	{
		//GEngine->AddOnScreenDebugMessage(INDEX_NONE,5.f,FColor::Red,"Enemy touching "+OtherActor->GetActorLabel());
		Cast<APot>(OtherActor)->ReceiveDamage(Damage);
	}
	
}

void AEnemy::OnWeaponEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	bTouchedPlayer=false;
	bTouchedPot=false;
}