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
    
    AActor* SelfActor = Cast<AActor>(MyBlackBoard->GetValueAsObject("SelfActor"));
    APlayerCharacter* Player = Cast<APlayerCharacter>(MyBlackBoard->GetValueAsObject("Player"));
    AActor* Target = Cast<AActor>(MyBlackBoard->GetValueAsObject("Target"));

    const float Distance =  MyBlackBoard->GetValueAsFloat("DistanceToPlayer");
    const float AttackRange = MyBlackBoard->GetValueAsFloat("AttackRange");
    const int MaxEnemiesOnPlayer = MyBlackBoard->GetValueAsInt("MaxEnemiesOnPlayer");
    
    AAIController* MyController = Cast<AAIController>(Cast<APawn>(SelfActor)->GetController());
    
    // if the player is in range of the enemy
    
    if(Distance<AttackRange && Player->GetEnemyNumber() < MaxEnemiesOnPlayer )
        MyBlackBoard->SetValueAsObject("FocusActor",Cast<AActor>(Player));
    else
        MyBlackBoard->SetValueAsObject("FocusActor",Target);

    AActor* FocusActor = Cast<AActor>(MyBlackBoard->GetValueAsObject("FocusActor"));
    
    // Set the focus towards new/old focus actor
    MyController->ClearFocus(EAIFocusPriority::Default);
    MyController->SetFocus(FocusActor,EAIFocusPriority::Default);

    GEngine->AddOnScreenDebugMessage(INDEX_NONE,5.f,FColor::Green,FocusActor->GetActorLabel());

    
    if(MyController->MoveToActor(FocusActor, MyBlackBoard->GetValueAsFloat("MarginRadius")))
        return EBTNodeResult::InProgress;
    return EBTNodeResult::Failed;
}

void UBTT_ChaseTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    UBlackboardComponent* MyBlackBoard = OwnerComp.GetBlackboardComponent();
    
    AActor* SelfActor = Cast<AActor>(MyBlackBoard->GetValueAsObject("SelfActor")); 
    APlayerCharacter* Player = Cast<APlayerCharacter>(MyBlackBoard->GetValueAsObject("Player"));
    AActor* Target = Cast<AActor>(MyBlackBoard->GetValueAsObject("Player"));
    AActor* FocusActor = Cast<AActor>(MyBlackBoard->GetValueAsObject("FocusActor"));
    
    const int MaxEnemiesOnPlayer = MyBlackBoard->GetValueAsInt("MaxEnemiesOnPlayer");
    const float Distance = MyBlackBoard->GetValueAsFloat("DistanceToPlayer");
    const float AttackRange = MyBlackBoard->GetValueAsFloat("AttackRange");
    
    AAIController* MyController = Cast<AAIController>(Cast<APawn>(SelfActor)->GetController());

    MyController->ClearFocus(EAIFocusPriority::Default);
    MyController->SetFocus(Player,EAIFocusPriority::Default);
    
    switch (MyController->GetMoveStatus()) {
    case EPathFollowingStatus::Idle:
        {
            // Character moved successfully towards goal, switching to attack mode
            MyBlackBoard->SetValueAsBool("Walking",false);
            MyBlackBoard->SetValueAsBool("Attacking",true);
            GEngine->AddOnScreenDebugMessage(INDEX_NONE,5.f,FColor::Yellow,"Idle state");

            return FinishLatentTask(OwnerComp,EBTNodeResult::Succeeded);
        }
    case EPathFollowingStatus::Waiting:
        // Honestly don't know what state it is
        GEngine->AddOnScreenDebugMessage(INDEX_NONE,5.f,FColor::Yellow,"Waiting State on player");
        return FinishLatentTask(OwnerComp,EBTNodeResult::Failed);
    case EPathFollowingStatus::Paused:
        GEngine->AddOnScreenDebugMessage(INDEX_NONE,5.f,FColor::Yellow,"Waiting State on player");
        // We paused the movement for whatever reason
        return FinishLatentTask(OwnerComp,EBTNodeResult::InProgress);
    case EPathFollowingStatus::Moving:
        {
            // The enemy is moving towards it's target

            //We get the unique ID of current focused actor
            const uint32 OldID = FocusActor->GetUniqueID();
            
            // if the player is getting in range of the enemy whilst it is moving
            if(Distance<AttackRange && Player->GetEnemyNumber() < MaxEnemiesOnPlayer )
                MyBlackBoard->SetValueAsObject("FocusActor",Cast<AActor>(Player));
            else
                MyBlackBoard->SetValueAsObject("FocusActor",Target);

            // Set the focus towards new/old focus actor
            MyController->SetFocus(FocusActor,EAIFocusPriority::Default);

            //If the focus has changed, move towards new focus
            if(OldID != FocusActor->GetUniqueID())
            {
                MyController->StopMovement();
                if(MyController->MoveToActor(FocusActor, MyBlackBoard->GetValueAsFloat("MarginRadius")))
                    return FinishLatentTask(OwnerComp,EBTNodeResult::InProgress);
                return FinishLatentTask(OwnerComp,EBTNodeResult::Failed);
            }

            return FinishLatentTask(OwnerComp,EBTNodeResult::InProgress);
        }
    default:
        return FinishLatentTask(OwnerComp,EBTNodeResult::Failed);
    }
}
