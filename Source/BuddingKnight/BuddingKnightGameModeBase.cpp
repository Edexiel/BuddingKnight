// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "BuddingKnightGameModeBase.h"
#include "PlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"


ABuddingKnightGameModeBase::ABuddingKnightGameModeBase()
{
    
    // set default pawn class to our Blueprinted characterBlueprint'/Game/BuddingKnight/MyPlayerCharacter2.MyPlayerCharacter2'
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/BuddingKnight/BP_PlayerCharacter"));

    if (PlayerPawnBPClass.Class != nullptr)
    {
        DefaultPawnClass = PlayerPawnBPClass.Class;
    }
    
}
