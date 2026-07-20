// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Monster/EnemyBase.h"
#include "Engine/Engine.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

AEnemyAIController::AEnemyAIController()
{
	ConstructorHelpers::FObjectFinder<UBehaviorTree>EnemyBT(TEXT("/Script/AIModule.BehaviorTree'/Game/ChaosSlash/AI/BT_Enemy.BT_Enemy'"));
	if (EnemyBT.Succeeded())
	{
		EnemyBehaviorTree = EnemyBT.Object;
	}
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (EnemyBehaviorTree != nullptr)
	{
		RunBehaviorTree(EnemyBehaviorTree);

		Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		
	}
}

void AEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 리스폰 후 새 폰을 반영하기 위해 매 틱마다 갱신
	APawn* CurrentPlayer = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	Player = CurrentPlayer;
	PlayerPawn = CurrentPlayer;

	if (!Player || !GetBlackboardComponent() || !GetPawn()) return;

	GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), Player->GetActorLocation());

	FVector MyLocation = GetPawn()->GetActorLocation();
	FVector PlayerLocation = PlayerPawn->GetActorLocation();
	float Distance = FVector::Dist(MyLocation, PlayerLocation);

	if (Distance <= 200.0f)
	{
		GetBlackboardComponent()->SetValueAsBool(TEXT("bIsAttack"), true);
	}
	else
	{
		GetBlackboardComponent()->SetValueAsBool(TEXT("bIsAttack"), false);
	}
}