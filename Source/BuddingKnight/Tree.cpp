// Fill out your copyright notice in the Description page of Project Settings.


#include "Tree.h"

#include "Enemy.h"
#include "PlayerCharacter.h"
#include "TimerManager.h"
#include "Components/BoxComponent.h"
#include "Engine/Engine.h"
#include "Kismet/KismetSystemLibrary.h"

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
    
    AttackBox->SetCollisionEnabled( ECollisionEnabled::Type::NoCollision);
    AttackBox->OnComponentBeginOverlap.AddDynamic(this, &ATree::OnAttackBoxBeginOverlap);
    AttackBox->OnComponentEndOverlap.AddDynamic(this, &ATree::OnAttackBoxEndOverlap);

    IsResettingEndAttackDelay = true;
}

void ATree::Passive(APlayerCharacter* Player)
{
    if(!Player)
        return;

    Player->SetBonusDamage(DamageBonus);
    //To do: void ChangeSword(ESwords::E_Tree);
}

void ATree::Special()
{
    AttackBox->SetCollisionEnabled( ECollisionEnabled::Type::QueryOnly);
    EndAttackDelay();
}

void ATree::OnAttackBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    
    if(OtherActor->IsA(AEnemy::StaticClass()))
    {
        GEngine->AddOnScreenDebugMessage(NULL,2.f,FColor::Red,TEXT("ATTACK"));
        AEnemy* Enemy = Cast<AEnemy>(OtherActor);
        if(!Enemy->IsAlreadyTakeDamage)
        {
            GEngine->AddOnScreenDebugMessage(NULL,2.f,FColor::Red,TEXT("GET HIT"));
            Enemy->ReceiveDamage(1.f);
            Enemy->IsAlreadyTakeDamage = true;
        }
    }
}

void ATree::OnAttackBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if(OtherActor->IsA(AEnemy::StaticClass()))
    {
        GEngine->AddOnScreenDebugMessage(NULL,2.f,FColor::Red,TEXT("ENEMY NOT IN THE BOX"));
        AEnemy* Enemy = Cast<AEnemy>(OtherActor);
        
        if(Enemy->IsAlreadyTakeDamage)
            Enemy->IsAlreadyTakeDamage = false;
    }
}

void ATree::EndAttackDelay()
{
    if(IsResettingEndAttackDelay)
    {
        GetWorldTimerManager().ClearTimer(TimeHandleEndAttackDelay);
        GetWorldTimerManager().SetTimer(TimeHandleEndAttackDelay, this, &ATree::ResetEndAttackDelay, AnimationCooldown, false);
        IsResettingEndAttackDelay = false;
    }
}

void ATree::ResetEndAttackDelay()
{
    UE_LOG(LogTemp, Warning, TEXT("BONSOIR"));
    IsResettingEndAttackDelay = true;
    AttackBox->SetCollisionEnabled( ECollisionEnabled::Type::NoCollision);
}
