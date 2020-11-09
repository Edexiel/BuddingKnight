// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ChaseTarget.h"


#include "AIController.h"
#include "PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type UBTT_ChaseTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UBlackboardComponent* MyBlackBoard = OwnerComp.GetBlackboardComponent();
    
    const AActor* SelfActor = Cast<AActor>(MyBlackBoard->GetValueAsObject("SelfActor"));
    APlayerCharacter* Player = Cast<APlayerCharacter>(MyBlackBoard->GetValueAsObject("Player"));
    AActor* Target = Cast<AActor>(MyBlackBoard->GetValueAsObject("Target"));

    const float Distance =  MyBlackBoard->GetValueAsFloat("DistanceToPlayer");
    const float AttackRange = MyBlackBoard->GetValueAsFloat("AttackRange");
    
    AAIController* MyController = Cast<AAIController>(Cast<APawn>(SelfActor)->GetController());

    if(!IsValid(MyController))
        return EBTNodeResult::Failed;
    
    // if the player is in range of the enemy 
    if(Distance<AttackRange && Player->IsTargetingPlayer(SelfActor->GetInstigator()))
        MyBlackBoard->SetValueAsObject("FocusActor",Cast<AActor>(Player));
    else
        MyBlackBoard->SetValueAsObject("FocusActor",Target);

    AActor* FocusActor = Cast<AActor>(MyBlackBoard->GetValueAsObject("FocusActor"));
    
    // Set the focus towards new/old focus actor
    MyController->ClearFocus(EAIFocusPriority::Gameplay);
    MyController->SetFocus(FocusActor);
    
    if(MyController->MoveToActor(FocusActor, MyBlackBoard->GetValueAsFloat("MarginRadius")))
        return EBTNodeResult::InProgress;
    return EBTNodeResult::Failed;
}

void UBTT_ChaseTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    UBlackboardComponent* MyBlackBoard = OwnerComp.GetBlackboardComponent();
    
    const AActor* SelfActor = Cast<AActor>(MyBlackBoard->GetValueAsObject("SelfActor")); 
    APlayerCharacter* Player = Cast<APlayerCharacter>(MyBlackBoard->GetValueAsObject("Player"));
    AActor* Target = Cast<AActor>(MyBlackBoard->GetValueAsObject("Target"));
    const AActor* FocusActor = Cast<AActor>(MyBlackBoard->GetValueAsObject("FocusActor"));
    
    const float Distance = MyBlackBoard->GetValueAsFloat("DistanceToPlayer");
    const float AttackRange = MyBlackBoard->GetValueAsFloat("AttackRange");
    
    AAIController* MyController = Cast<AAIController>(Cast<APawn>(SelfActor)->GetController());

    if(!IsValid(MyController))
        return FinishLatentTask(OwnerComp,EBTNodeResult::Failed);
    
    switch (MyController->GetMoveStatus()) {

    //todo check in idle if the target is the player, if is the player and not close = unreachable -> focus target
    case EPathFollowingStatus::Idle:
        //Player is maybe not on navmesh
        // Character moved successfully towards goal, switching to attack mode
        MyBlackBoard->SetValueAsBool("Walking",false);
        MyBlackBoard->SetValueAsBool("Attacking",true);
        return FinishLatentTask(OwnerComp,EBTNodeResult::Succeeded);
    case EPathFollowingStatus::Waiting:
        // Honestly don't know what state it is
        return FinishLatentTask(OwnerComp,EBTNodeResult::Failed);
    case EPathFollowingStatus::Paused:
        // We paused the movement for whatever reason
        return FinishLatentTask(OwnerComp,EBTNodeResult::InProgress);
    case EPathFollowingStatus::Moving:
        {

            const bool bCloseAndValid = Distance < AttackRange && Player->IsTargetingPlayer(SelfActor->GetInstigator()) ;
            // The enemy is moving towards it's target
            if(FocusActor==Target)
            {
                //if the player come close and  had a valid path
                if(bCloseAndValid)
                {
                    MyBlackBoard->SetValueAsObject("FocusActor",Cast<AActor>(Player));
                    
                    MyController->ClearFocus(EAIFocusPriority::Gameplay);
                    MyController->SetFocus(Player);

                    EPathFollowingRequestResult::Type PathResult = MyController->MoveToActor(Player, MyBlackBoard->GetValueAsFloat("MarginRadius"));

                    if( PathResult==EPathFollowingRequestResult::RequestSuccessful )
                        return FinishLatentTask(OwnerComp,EBTNodeResult::InProgress);                
                    
                    MyController->ClearFocus(EAIFocusPriority::Gameplay);
                    MyController->SetFocus(Target);
                    
                    //GEngine->AddOnScreenDebugMessage(INDEX_NONE,5.f,FColor::Red,"Player out of bounds");

                    return FinishLatentTask(OwnerComp,EBTNodeResult::Failed);
                }
            return FinishLatentTask(OwnerComp,EBTNodeResult::InProgress);                

            }
                // The Enemy is moving towards the player
            //the player is not in reach anymore or too much enemies on player, refocus towards target
            if(!bCloseAndValid)
            {
                //GEngine->AddOnScreenDebugMessage(INDEX_NONE,5.f,FColor::Green,"Refocus towards : Target");
                MyBlackBoard->SetValueAsObject("FocusActor",Target);
                    
                MyController->ClearFocus(EAIFocusPriority::Gameplay);
                MyController->SetFocus(Target);

                if(MyController->MoveToActor(Target, MyBlackBoard->GetValueAsFloat("MarginRadius")))
                    return FinishLatentTask(OwnerComp,EBTNodeResult::InProgress);                
                //return FinishLatentTask(OwnerComp,EBTNodeResult::Failed);

            }
            return FinishLatentTask(OwnerComp,EBTNodeResult::InProgress);                
        }
    default:
        return FinishLatentTask(OwnerComp,EBTNodeResult::Failed);
    }
}
