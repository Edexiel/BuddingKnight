// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ActorComponent.h"
#include "BaseComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUDDINGKNIGHT_API UBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBaseComponent();

	UPROPERTY(EditAnywhere, Category="BaseComponent", meta=(AllowPrivateAccess="true"))
		class UBaseComponentDataAsset* BaseComponentDataAsset{nullptr};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BaseComponent", meta=(BindWidget))
		UUserWidget * Widget;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY(Category = "Stats", VisibleAnywhere, BlueprintReadWrite)
		float Health;
	
	UPROPERTY(Category = "Stats", VisibleAnywhere, BlueprintReadWrite)
		float MaxHealth;


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
		float GetHealth() const;
	
	UFUNCTION(BlueprintCallable)
		float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable)
        void TakeDamage(const float Damage);
};
