// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Attack.h"


#include "AIController.h"
#include "Enemy.h"
#include "PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

class APlayerCharacter;

EBTNodeResult::Type UBTT_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
     UBlackboardComponent* MyBlackBoard = OwnerComp.GetBlackboardComponent();
    
    APlayerCharacter* Player = Cast<APlayerCharacter>(MyBlackBoard->GetValueAsObject("Player"));
    AActor* SelfActor = Cast<AActor>(MyBlackBoard->GetValueAsObject("SelfActor"));
    AActor* Target = Cast<AActor>(MyBlackBoard->GetValueAsObject("Target"));
    AActor* FocusActor = Cast<AActor>(MyBlackBoard->GetValueAsObject("FocusActor"));


    AAIController* MyController = Cast<AAIController>(Cast<APawn>(SelfActor)->GetController());

    const float Distance = SelfActor->GetDistanceTo(FocusActor);
    
    const float MarginRadius = MyBlackBoard->GetValueAsFloat("MarginRadius");
    const float AttackRange = MyBlackBoard->GetValueAsFloat("AttackRange");

    //launch attack animation
    Cast<AEnemy>(SelfActor)->Attack();
    
    if(Distance > AttackRange && !Player->IsStun())
    {
        MyBlackBoard->SetValueAsObject("FocusActor",Target?Target:MyBlackBoard->GetValueAsObject("Player"));
        MyController->SetFocus(FocusActor);
        MyBlackBoard->SetValueAsBool("Walking",true);
        MyBlackBoard->SetValueAsBool("Attacking",false);
        return EBTNodeResult::Failed;
    }
    
    if(Distance > MarginRadius)
    {
        MyController->SetFocus(FocusActor);
        MyBlackBoard->SetValueAsBool("Walking",true);
        MyBlackBoard->SetValueAsBool("Attacking",false);
        return EBTNodeResult::Failed;
    }


    
    return EBTNodeResult::Succeeded;

    
}
