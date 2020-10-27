// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_Attack.generated.h"

/**
 * 
 */
UCLASS()
class BUDDINGKNIGHT_API UBTT_Attack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	explicit UBTT_Attack(const FObjectInitializer& ObjectInitializer)
		: UBTTaskNode(ObjectInitializer)
	{
		bNotifyTick=true;
	}

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
