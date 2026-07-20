// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BTTask_EnemyAttack.h"
#include "Player/PlayerCharacterBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Monster/EnemyBase.h"
#include "Monster/EnemyAIController.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "BTTask_EnemyAttack.h"


UBTTask_EnemyAttack::UBTTask_EnemyAttack()
{
	NodeName = "MonsterAttackTask";
	bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_EnemyAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("[MonsterAttack] AIController 없음 — 공격 실패"));
		return EBTNodeResult::Failed;
	}

	AEnemyBase* Enemy = Cast<AEnemyBase>(AIController->GetPawn());
	if (!Enemy)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("[MonsterAttack] EnemyBase Cast 실패 — 공격 실패"));
		return EBTNodeResult::Failed;
	}

	APlayerCharacterBase* Player = Cast<APlayerCharacterBase>(
		UGameplayStatics::GetPlayerPawn(Enemy->GetWorld(), 0));
	if (!Player)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("[MonsterAttack] Player 없음 — 공격 실패"));
		return EBTNodeResult::Failed;
	}

	// GetTimeSeconds()는 게임 시작 후 경과한 시간(초)을 반환한다
	// 현재 시각에서 마지막 공격 시각을 빼면 "마지막 공격으로부터 얼마나 흘렀는지"가 나온다
	float Now = Enemy->GetWorld()->GetTimeSeconds();
	float Elapsed = Now - Enemy->LastAttackTime;
	if (Elapsed < Enemy->AttackCooldown)
	{
		// 아직 쿨타임이 남아 있으면 데미지 없이 태스크만 성공으로 종료
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::White,
			FString::Printf(TEXT("[MonsterAttack] 쿨타임 중 (%.1f / %.1f초)"), Elapsed, Enemy->AttackCooldown));
		return EBTNodeResult::Succeeded;
	}

	float Distance = FVector::Dist(Enemy->GetActorLocation(), Player->GetActorLocation());
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange,
		FString::Printf(TEXT("[MonsterAttack] %s → Player | 거리: %.0f | 데미지: %.0f"),
			*Enemy->GetName(), Distance, Enemy->EnemyDamage));

	// 공격 범위 초과 시 데미지 없이 종료 (리스폰 후 원거리에서 데미지 들어오는 현상 방지)
	if (Distance > 200.0f)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
			TEXT("[MonsterAttack] 공격 범위 초과 — 데미지 취소"));
		return EBTNodeResult::Failed;
	}

	// 공격 직전에 현재 시각을 기록해 두면, 다음 ExecuteTask 호출 때 Elapsed 계산의 기준이 된다
	Enemy->LastAttackTime = Now;
	Enemy->Attack();
	UGameplayStatics::ApplyDamage(Player, Enemy->EnemyDamage, AIController, Enemy, UDamageType::StaticClass());

	return EBTNodeResult::Succeeded;
}

