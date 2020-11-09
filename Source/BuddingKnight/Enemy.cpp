// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"


#include "AIC_EnemyCAC.h"
#include "Components/CapsuleComponent.h"
#include "PlayerCharacter.h"
#include "TimerManager.h"
#include "Engine/Engine.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	
	BaseComponent = CreateDefaultSubobject<UBaseComponent>(TEXT("BaseComponent"));
	
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnCapsuleBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnCapsuleEndOverlap);
	
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AEnemy::IsDead() const
{
	return BaseComponent->IsDead();
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

void AEnemy::OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(IsGettingHit || IsDead())
		return;
	
	if(OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		if(!IsValid(OtherActor))
			return;
		
		//GEngine->AddOnScreenDebugMessage(INDEX_NONE,2.f,FColor::Red,"Enemy : Collision of "+GetActorLabel()+" and "+OtherActor->GetActorLabel());

		IsGettingHit = true;
		
		LaunchCharacter(GetActorForwardVector()*KnockBackForce*-1,true,true);
		const float RemainingTime = PlayAnimMontage(GettingHitAnimMontage);
		GetWorldTimerManager().SetTimer(GettingHitHandle,this,&AEnemy::ResetGettingHit,RemainingTime,false);

		//todo play sound
		BaseComponent->TakeDamage(Cast<APlayerCharacter>(OtherActor)->GetDamage());

		if(IsDead())
		{
			GetCapsuleComponent()->DestroyComponent();
			Cast<AAIC_EnemyCAC>(GetController())->SetDead();
			// GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic,ECollisionResponse::ECR_Block);
			GetWorldTimerManager().SetTimer(DestroyHandle,this,&AEnemy::Delete,DepopTime,false);
		}
	}
}

void AEnemy::OnCapsuleEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

