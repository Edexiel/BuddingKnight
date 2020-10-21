#include "AIC_EnemyCAC.h"

#include "PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

void AAIC_EnemyCAC::BeginPlay()
{
    Super::BeginPlay();

    if(BTAsset)
    {
        RunBehaviorTree(BTAsset);
        BB = GetBlackboardComponent();
    }   
    if (!BB->GetValueAsObject("Player"))
        BB->SetValueAsObject("Player",Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this,0)));

    if(!BB->GetValueAsFloat("AcceptanceRadius"))
        BB->SetValueAsFloat("AcceptanceRadius",DetectionRadius);

    if(!BB->GetValueAsFloat("MarginRadius"))
        BB->SetValueAsFloat("MarginRadius",MarginRadius);
}
