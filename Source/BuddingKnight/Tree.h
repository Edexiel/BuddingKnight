// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Plant.h"
#include "Tree.generated.h"


UCLASS()
class BUDDINGKNIGHT_API ATree : public APlant
{
	GENERATED_BODY()
public:
	ATree();

	virtual void BeginPlay() override;

    virtual void Passive(APlayerCharacter* Player) override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	class UBoxComponent* AttackBox;

	UPROPERTY(EditAnywhere, Category="Bonus")
	float DamageBonus{0};

	UPROPERTY(EditAnywhere, Category="Attack")
	float Damage{0};
	
	virtual void Special() override;

    UFUNCTION()
    	void OnAttackBoxBeginOverlap(class UPrimitiveComponent* OverlappedComp,
    	                    class AActor* OtherActor,
    	                    class UPrimitiveComponent* OtherComp,
    	                    int32 OtherBodyIndex,
    	                    bool bFromSweep,
    	                    const FHitResult& SweepResult);

	UFUNCTION()
    void OnAttackBoxEndOverlap(UPrimitiveComponent* OverlappedComp,
                        AActor* OtherActor,
                        UPrimitiveComponent* OtherComp,
                        int32 OtherBodyIndex);
	
private:

	UPROPERTY()
	bool IsResettingEndAttackDelay;
	
	UPROPERTY()
	FTimerHandle TimeHandleEndAttackDelay;

	UFUNCTION()
    void EndAttackDelay();

	UFUNCTION()
    void ResetEndAttackDelay();
};
