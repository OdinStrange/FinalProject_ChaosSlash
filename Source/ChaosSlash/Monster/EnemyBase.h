// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

class APotionPickup;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyBaseDied);

UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	NORMALENEMY UMETA(DisplayName = "Zombie"),
	MAGICENEMY UMETA(DisplayName = "Juggernaut"),
};

UCLASS()
class CHAOSSLASH_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;
	//ApplyDamage 호출 시 엔진이 이 함수를 실행해 CurrentEnemyHp를 깎는다

public:
	void Attack();

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* AttackAction;

public:
	UPROPERTY(EditAnywhere)
	EEnemyType CurrentEnemyType = EEnemyType::NORMALENEMY;

	UPROPERTY(EditAnywhere)
	TMap<EEnemyType, USkeletalMesh*> EnemyMeshes;

	UPROPERTY(EditAnywhere)
	TMap<EEnemyType, TSubclassOf<UAnimInstance>> EnemyAnimBlueprints;

	UPROPERTY(EditAnywhere)
	FVector EnemySize;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float MaxEnemyHp; //에디터에서 몬스터 타입별 최대 HP 설정

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	float CurrentEnemyHp; //런타임 중 실제로 깎이는 HP

	UPROPERTY(EditAnywhere)
	float EnemyDamage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackCooldown = 1.5f; //공격과 공격 사이에 필요한 최소 간격(초) — 에디터에서 몬스터별로 조정

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* AttackMontage;

	// GetWorld()->GetTimeSeconds()로 기록한 마지막 공격 시각(게임 시작 후 경과 초)
	// -999로 초기화하면 (현재 시각 - (-999)) 가 항상 쿨타임보다 크므로 게임 시작 직후 첫 공격이 바로 가능
	float LastAttackTime = -999.f;

	/** 사망 시 외부(스포너 등)에 알리는 델리게이트 */
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnEnemyBaseDied OnEnemyDied;

	/** 피격 시 랜덤 재생할 사운드 목록 */
	UPROPERTY(EditAnywhere, Category="Audio")
	TArray<USoundBase*> HitSounds;

	/** 사망 시 드랍할 픽업 클래스 (블루프린트 자식에서 지정) */
	UPROPERTY(EditAnywhere, Category="Drop")
	TSubclassOf<APotionPickup> DropPickupClass;

	/** 드랍 확률 (0=절대 안 드랍, 1=항상 드랍) */
	UPROPERTY(EditAnywhere, Category="Drop", meta=(ClampMin=0, ClampMax=1))
	float DropChance = 0.05f;

public:
	void InitializeMeshes();
	void InitializeAnimInstance();
	void InitializeSize();
	void InitializeHp();
	void InitializeDamage();
	void InitializeAIController();
};
