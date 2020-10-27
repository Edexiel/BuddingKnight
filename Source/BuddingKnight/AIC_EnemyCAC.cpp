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

    if(!Blackboard->GetValueAsFloat("AttackRange"))
        Blackboard->SetValueAsFloat("AttackRange",AttackRange);

    if(!Blackboard->GetValueAsFloat("MarginRadius"))
        Blackboard->SetValueAsFloat("MarginRadius",MarginRadius);

    if(!Blackboard->GetValueAsFloat("DepopTime"))
        Blackboard->SetValueAsFloat("DepopTime",DepopTime);

    if(!Blackboard->GetValueAsFloat("MaxEnemiesOnPlayer"))
        Blackboard->SetValueAsFloat("MaxEnemiesOnPlayer",MaxEnemiesOnPlayer);

    Blackboard->SetValueAsBool("Walking",true);
    Blackboard->SetValueAsBool("Attacking",false);
    Blackboard->SetValueAsBool("isAlive",true);
    
    Blackboard->SetValueAsFloat("WaitTimeBetweenAttacks",WaitTimeBetweenAttacks);
    if(!Blackboard->GetValueAsFloat("DistanceToPlayer"))
        Blackboard->SetValueAsFloat("DistanceToPlayer",GetPawn()->GetDistanceTo(PlayerCharacter));

    
    UCharacterMovementComponent* MovementComponent = GetPawn()->GetController()->GetCharacter()->GetCharacterMovement();  
    MovementComponent->MaxWalkSpeed = MaxWalkSpeed;
    MovementComponent->SetUpdateNavAgentWithOwnersCollisions(true);
    MovementComponent->SetAvoidanceEnabled(true);
    
}

void AAIC_EnemyCAC::SetTarget(AActor* Actor)
{
    Target = Actor;
    
    Blackboard->SetValueAsObject("Target",Target);
    Blackboard->SetValueAsObject("FocusActor",Target);
    SetFocus(Target,EAIFocusPriority::Default);
}

void AAIC_EnemyCAC::Tick(float DeltaTime)
{
    Blackboard->SetValueAsFloat("DistanceToPlayer",GetPawn()->GetDistanceTo(PlayerCharacter));
}
