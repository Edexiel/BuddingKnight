#include "AIC_EnemyCAC.h"

#include "PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Kismet/GameplayStatics.h"

void AAIC_EnemyCAC::BeginPlay()
{
    Super::BeginPlay();

    if(BTAsset)
    {
        RunBehaviorTree(BTAsset);
        Blackboard = GetBlackboardComponent();
    }
    
    PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this,0));
    
    if (!Blackboard->GetValueAsObject("Player"))
        Blackboard->SetValueAsObject("Player",PlayerCharacter);

    if(!Blackboard->GetValueAsFloat("DetectionRadius"))
        Blackboard->SetValueAsFloat("DetectionRadius",DetectionRadius);

    if(!Blackboard->GetValueAsFloat("MarginRadius"))
        Blackboard->SetValueAsFloat("MarginRadius",MarginRadius);

    if(!Blackboard->GetValueAsFloat("DepopTime"))
        Blackboard->SetValueAsFloat("DepopTime",DepopTime);

    if(!Blackboard->GetValueAsFloat("DistanceFromPlayer"))
        Blackboard->SetValueAsFloat("DistanceFromPlayer",GetPawn()->GetDistanceTo(PlayerCharacter));

    UCharacterMovementComponent* MovementComponent= GetPawn()->GetController()->GetCharacter()->GetCharacterMovement();
    MovementComponent->SetUpdateNavAgentWithOwnersCollisions(true);
    MovementComponent->SetAvoidanceEnabled(true);

 
}

void AAIC_EnemyCAC::Tick(float DeltaTime)
{
    Blackboard->SetValueAsFloat("DistanceFromPlayer",GetPawn()->GetDistanceTo(PlayerCharacter));
}
