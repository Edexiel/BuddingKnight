#include "AIC_EnemyCAC.h"

#include "PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/Engine.h"
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
    

    Blackboard->SetValueAsObject("Player",PlayerCharacter);
    Blackboard->SetValueAsFloat("AttackRange",AttackRange);
    Blackboard->SetValueAsFloat("MarginRadius",MarginRadius);
    Blackboard->SetValueAsFloat("DepopTime",DepopTime);
    Blackboard->SetValueAsInt("MaxEnemiesOnPlayer",MaxEnemiesOnPlayer);
    
    Blackboard->SetValueAsBool("Walking",true);
    Blackboard->SetValueAsBool("Attacking",false);
    Blackboard->SetValueAsBool("isAlive",true);
    
    Blackboard->SetValueAsFloat("WaitTimeBetweenAttacks",WaitTimeBetweenAttacks);
    // if(!Blackboard->GetValueAsFloat("DistanceToPlayer"))
    //     Blackboard->SetValueAsFloat("DistanceToPlayer",GetPawn()->GetDistanceTo(PlayerCharacter));
   
}

void AAIC_EnemyCAC::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
}

void AAIC_EnemyCAC::SetTarget(AActor* Actor)
{
    Target = Actor;
    
    Blackboard->SetValueAsObject("Target",Target);
    Blackboard->SetValueAsObject("FocusActor",Target);
    SetFocus(Target,EAIFocusPriority::Default);
    GEngine->AddOnScreenDebugMessage(INDEX_NONE,2.f,FColor::Red,"Target set");
}

void AAIC_EnemyCAC::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if(!Init)
    {
        UCharacterMovementComponent* MovementComponent = GetCharacter()->GetCharacterMovement();  
        MovementComponent->MaxWalkSpeed = MaxWalkSpeed;
        MovementComponent->SetUpdateNavAgentWithOwnersCollisions(true);
        MovementComponent->SetAvoidanceEnabled(true);

        Init=true;
    }
    
    const float Distance = GetPawn()->GetDistanceTo(PlayerCharacter);

    //todo :: move that into chaseTarget so that HasPartialPath can work
    if(HasPartialPath())
    {
        //Player is not on navmesh
        GEngine->AddOnScreenDebugMessage(INDEX_NONE,5.f,FColor::Red,"Player not on navmesh");

        Blackboard->SetValueAsFloat("DistanceToPlayer",999999);
        ClearFocus(EAIFocusPriority::Default);
        SetFocus(Target);
    }
    else
    {
        //Player is on navmesh
        Blackboard->SetValueAsFloat("DistanceToPlayer",Distance);
    }
    
    if(Blackboard->GetValueAsFloat("DistanceToPlayer") < AttackRange)
        PlayerCharacter->RegisterEnemy(GetPawn());
    else
        PlayerCharacter->UnregisterEnemy(GetPawn());
}
