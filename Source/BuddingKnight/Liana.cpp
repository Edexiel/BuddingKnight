// Fill out your copyright notice in the Description page of Project Settings.


#include "Liana.h"


#include "AIC_EnemyCAC.h"
#include "Enemy.h"

#include "PlayerCharacter.h"
#include "TimerManager.h"
#include "Kismet/KismetSystemLibrary.h"

void ALiana::UnFreeze()
{
    for(AEnemy* Enemy : Enemies)
    {
        if(IsValid(Enemy))
            Enemy->UnFreeze();

    }
}

void ALiana::Special()
{
    for(AEnemy* Enemy : Enemies)
    {
        if(IsValid(Enemy))
            Enemy->Freeze();
    }
    
    GetWorldTimerManager().SetTimer(FreezeHandle,this,&ALiana::UnFreeze,UnFreezeTimer,false);
}

void ALiana::Passive(APlayerCharacter* Player)
{
    if(!Player)
        return;
    
   Player->ChangeSword(ESwords::E_Plant);
}
