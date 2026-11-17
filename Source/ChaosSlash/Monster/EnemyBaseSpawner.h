#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Monster/EnemyBase.h"
#include "Variant_Combat/Interfaces/CombatActivatable.h"
#include "EnemyBaseSpawner.generated.h"

class UCapsuleComponent;
class UArrowComponent;

USTRUCT(BlueprintType)
struct FEnemySpawnEntry
{
	GENERATED_BODY()

	/** 생성할 적 블루프린트 클래스 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	TSubclassOf<AEnemyBase> EnemyClass;

	/** 생성할 적 종류 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	EEnemyType EnemyType = EEnemyType::NORMALENEMY;

	/** 해당 종류의 생성 수량 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy", meta = (ClampMin = 0, ClampMax = 100))
	int32 SpawnCount = 1;
};

UCLASS()
class CHAOSSLASH_API AEnemyBaseSpawner
	: public AActor
	, public ICombatActivatable
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* SpawnCapsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* SpawnDirection;

protected:

	/** 생성할 적 종류 목록 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Spawner")
	TArray<FEnemySpawnEntry> EnemySpawnEntries;

	/** 게임 시작 즉시 스폰할지 여부 */
	UPROPERTY(EditAnywhere, Category = "Enemy Spawner")
	bool bShouldSpawnEnemiesImmediately = true;

	/** 첫 적 생성까지 대기 시간 */
	UPROPERTY(EditAnywhere, Category = "Enemy Spawner", meta = (ClampMin = 0, ClampMax = 10, Units = "s"))
	float InitialSpawnDelay = 2.0f;

	/** 다음 적 생성까지 대기 시간 */
	UPROPERTY(EditAnywhere, Category = "Enemy Spawner", meta = (ClampMin = 0, ClampMax = 10, Units = "s"))
	float RespawnDelay = 8.0f;

	/** 마지막 적 사망 후 다음 액터 활성화까지 대기 시간 */
	UPROPERTY(EditAnywhere, Category = "Activation", meta = (ClampMin = 0, ClampMax = 10, Units = "s"))
	float ActivationDelay = 1.0f;

	/** 스포너가 소진되었을 때 활성화할 액터 목록 */
	UPROPERTY(EditAnywhere, Category = "Activation")
	TArray<AActor*> ActorsToActivateWhenDepleted;

	/** 현재 처리할 스폰 배열 항목 */
	int32 CurrentSpawnEntryIndex = 0;

	/** 현재 항목에서 아직 생성하지 않은 적 수 */
	int32 CurrentEntryRemainingCount = 0;

	/** 전체 남은 생성 수 */
	int32 RemainingSpawnCount = 0;

	bool bHasBeenActivated = false;
	bool bSpawnerExhausted = false;
	int32 LiveEnemyCount = 0;

	FTimerHandle SpawnTimer;
	FTimerHandle ClearCheckTimer;

	void CheckClearCondition();

public:

	bool IsExhausted() const
	{
		return bSpawnerExhausted;
	}

	AEnemyBaseSpawner();

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

protected:

	void SpawnEnemy();

	UFUNCTION()
	void OnEnemyDied();

	void SpawnerDepleted();

public:

	virtual void ToggleInteraction(AActor* ActivationInstigator) override;
	virtual void ActivateInteraction(AActor* ActivationInstigator) override;
	virtual void DeactivateInteraction(AActor* ActivationInstigator) override;
};
