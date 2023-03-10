// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Plant.h"
#include "Spore.generated.h"

UCLASS()
class BUDDINGKNIGHT_API ASpore : public APlant
{
	GENERATED_BODY()
public:
	ASpore();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* SpawnPointProjectile;

	virtual void Passive(APlayerCharacter* Player) override;

	UFUNCTION()
	void DestroyPassiveSpore();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile", meta=(AllowPrivateAccess="true"))
	TSubclassOf<class ASporeProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Passive", meta=(AllowPrivateAccess="true"))
	TSubclassOf<class APassiveSpore> PassiveSporeClass;

	UPROPERTY()
	class APassiveSpore* PassiveSpore;

	
	UFUNCTION()
	virtual void Special() override;

	UFUNCTION(BlueprintCallable)
	void SpawnProjectile() const;
};
