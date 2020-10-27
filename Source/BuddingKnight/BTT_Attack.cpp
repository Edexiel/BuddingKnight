// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Attack.h"


#include "AIController.h"
#include "PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

class APlayerCharacter;

EBTNodeResult::Type UBTT_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
     UBlackboardComponent* MyBlackBoard = OwnerComp.GetBlackboardComponent();
    
    AActor* SelfActor = Cast<AActor>(MyBlackBoard->GetValueAsObject("SelfActor"));
    APlayerCharacter* Player = Cast<APlayerCharacter>(MyBlackBoard->GetValueAsObject("Player"));
    AActor* Target = Cast<AActor>(MyBlackBoard->GetValueAsObject("Target"));
    AActor* FocusActor = Cast<AActor>(MyBlackBoard->GetValueAsObject("FocusActor"));


    AAIController* MyController = Cast<AAIController>(Cast<APawn>(SelfActor)->GetController());

    const float Distance = SelfActor->GetDistanceTo(FocusActor);
    
    const float MarginRadius = MyBlackBoard->GetValueAsFloat("MarginRadius");
    const float AttackRange = MyBlackBoard->GetValueAsFloat("AttackRange");
    const int MaxEnemiesOnPlayer = MyBlackBoard->GetValueAsInt("MaxEnemiesOnPlayer");
    
    if(Distance > AttackRange)
    {
        MyBlackBoard->SetValueAsObject("FocusActor",Target?Target:MyBlackBoard->GetValueAsObject("Player"));
        MyController->SetFocus(FocusActor);
        MyBlackBoard->SetValueAsBool("Walking",true);
        MyBlackBoard->SetValueAsBool("Attacking",false);
        return EBTNodeResult::Failed;
    }
    
    if(Distance > MarginRadius)
    {
        //MyBlackBoard->SetValueAsObject("FocusActor",Cast<AActor>(Target?Target:Player));
        MyController->SetFocus(FocusActor);
        MyBlackBoard->SetValueAsBool("Walking",true);
        MyBlackBoard->SetValueAsBool("Attacking",false);
        return EBTNodeResult::Failed;
    }

    //launch attack animation 
    
    return EBTNodeResult::Succeeded;

    
}
