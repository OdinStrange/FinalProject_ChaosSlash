// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class CHAOSSLASH_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyAIController();

protected:
	virtual void BeginPlay() override;

	APawn* PlayerPawn;

	UPROPERTY(EditAnywhere)
	class UBehaviorTree* EnemyBehaviorTree;

	APawn* Player;

public:
	virtual void Tick(float Deltaseconds) override;

};
