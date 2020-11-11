// Fill out your copyright notice in the Description page of Project Settings.


#include "Liana.h"


#include "PlayerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

void ALiana::Special()
{
}

void ALiana::Passive(APlayerCharacter* Player)
{
    if(!Player)
        return;
    
   Player->ChangeSword(ESwords::E_Plant);
}
