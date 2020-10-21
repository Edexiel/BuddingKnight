// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ChaseTarget.h"


#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

EBTNodeResult::Type UBTT_ChaseTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UBlackboardComponent* MyBlackBoard = OwnerComp.GetBlackboardComponent();

    AActor* SelfActor = Cast<AActor>(MyBlackBoard->GetValueAsObject("SelfActor"));
    AActor* PlayerActor = Cast<AActor>(MyBlackBoard->GetValueAsObject("EnemyActor"));

    if(!PlayerActor || !SelfActor)
        return EBTNodeResult::Failed;
    
    AAIController* MyController = Cast<AAIController>(Cast<APawn>(SelfActor)->GetController());
    MyController->GetCharacter()->GetCharacterMovement()->SetUpdateNavAgentWithOwnersCollisions(true);
    MyController->GetCharacter()->GetCharacterMovement()->SetAvoidanceEnabled(true);
    

    if(!MyController)
        UE_LOG(LogTemp,Error,TEXT("MYCONTROLLER NON INIT"));

    const float distance = PlayerActor->GetDistanceTo(SelfActor);
    MyBlackBoard->SetValueAsFloat("DistanceToPlayer",distance);

    if(Cast<ABaseCharacter>(SelfActor)->IsDead())
    {
        MyController->ClearFocus(EAIFocusPriority::Gameplay);
        MyController->ClearFocus(EAIFocusPriority::Default);
        MyController->SetFocus(SelfActor,EAIFocusPriority::Default);
        return EBTNodeResult::Succeeded;
    }
    
    if(distance < MyBlackBoard->GetValueAsFloat("AcceptanceRadius"))
    {
        MyBlackBoard->SetValueAsBool("Retreating",true);
        
        FVector direction = SelfActor->GetActorLocation()-PlayerActor->GetActorLocation();
        direction.Normalize();
        direction*=MyBlackBoard->GetValueAsFloat("AcceptanceRadius");
        
        MyController->MoveTo({SelfActor->GetActorLocation()+direction});
        return EBTNodeResult::InProgress;
    }

    if(MyController->MoveToActor(PlayerActor, MyBlackBoard->GetValueAsFloat("AcceptanceRadius")))
        return EBTNodeResult::InProgress;

    return EBTNodeResult::Failed;
}

void UBTT_ChaseTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    UBlackboardComponent* MyBlackBoard = OwnerComp.GetBlackboardComponent();
    AActor* SelfActor = Cast<AActor>(MyBlackBoard->GetValueAsObject("SelfActor")); 
    AActor* PlayerActor = Cast<AActor>(MyBlackBoard->GetValueAsObject("Player"));
    AAIController* MyController = Cast<AAIController>(Cast<APawn>(SelfActor)->GetController()); // Nain controller
    
    const float distance = PlayerActor->GetDistanceTo(SelfActor);
    MyBlackBoard->SetValueAsFloat("DistanceToPlayer",distance);

    MyController->SetFocus(PlayerActor,EAIFocusPriority::Default);

    switch (MyController->GetMoveStatus()) {
    case EPathFollowingStatus::Idle:
        if (MyBlackBoard->GetValueAsBool("Retreating"))
            MyBlackBoard->SetValueAsBool("Retreating",false);
        return FinishLatentTask(OwnerComp,EBTNodeResult::Succeeded);
    case EPathFollowingStatus::Waiting:
        return FinishLatentTask(OwnerComp,EBTNodeResult::Failed);
    case EPathFollowingStatus::Paused:
        return FinishLatentTask(OwnerComp,EBTNodeResult::InProgress);
    case EPathFollowingStatus::Moving:
        {
            if(MyBlackBoard->GetValueAsBool("Retreating"))
            {
                MyController->SetFocus(PlayerActor);
                return FinishLatentTask(OwnerComp,EBTNodeResult::InProgress);
            }         
            return FinishLatentTask(OwnerComp,EBTNodeResult::InProgress);
        }
    default:
        return FinishLatentTask(OwnerComp,EBTNodeResult::Failed);
    }
}
