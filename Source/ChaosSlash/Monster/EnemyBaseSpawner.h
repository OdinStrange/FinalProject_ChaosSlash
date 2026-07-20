#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Variant_Combat/Interfaces/CombatActivatable.h"
#include "EnemyBaseSpawner.generated.h"

class AEnemyBase;
class UCapsuleComponent;
class UArrowComponent;

/**
 * AEnemyBase 전용 스포너.
 * CombatActivatable 인터페이스를 구현해 ActivationVolume 체인에 연결 가능.
 * 마지막 적이 죽으면 ActorsToActivateWhenDepleted 목록을 활성화.
 */
UCLASS()
class CHAOSSLASH_API AEnemyBaseSpawner : public AActor, public ICombatActivatable
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	UCapsuleComponent* SpawnCapsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	UArrowComponent* SpawnDirection;

protected:

	/** 스폰할 적 클래스 */
	UPROPERTY(EditAnywhere, Category="Enemy Spawner")
	TSubclassOf<AEnemyBase> EnemyClass;

	/** true면 게임 시작 즉시 스폰, false면 ActivationVolume이 켜줄 때까지 대기 */
	UPROPERTY(EditAnywhere, Category="Enemy Spawner")
	bool bShouldSpawnEnemiesImmediately = true;

	/** 첫 스폰까지 대기 시간 */
	UPROPERTY(EditAnywhere, Category="Enemy Spawner", meta=(ClampMin=0, ClampMax=10, Units="s"))
	float InitialSpawnDelay = 2.0f;

	/** 스폰할 총 적 수 */
	UPROPERTY(EditAnywhere, Category="Enemy Spawner", meta=(ClampMin=1, ClampMax=100))
	int32 SpawnCount = 30;

	/** 적 사망 후 다음 적 스폰까지 대기 시간 */
	UPROPERTY(EditAnywhere, Category="Enemy Spawner", meta=(ClampMin=0, ClampMax=10, Units="s"))
	float RespawnDelay = 8.0f;

	/** 마지막 적 사망 후 다음 액터 활성화까지 대기 시간 */
	UPROPERTY(EditAnywhere, Category="Activation", meta=(ClampMin=0, ClampMax=10, Units="s"))
	float ActivationDelay = 1.0f;

	/** 스포너가 소진되면 활성화할 액터 목록 (다음 스포너, 문 등) */
	UPROPERTY(EditAnywhere, Category="Activation")
	TArray<AActor*> ActorsToActivateWhenDepleted;

	bool bHasBeenActivated = false;
	bool bSpawnerExhausted = false;
	int32 LiveEnemyCount = 0;

	FTimerHandle SpawnTimer;
	FTimerHandle ClearCheckTimer;

	void CheckClearCondition();

public:
	bool IsExhausted() const { return bSpawnerExhausted; }

public:

	AEnemyBaseSpawner();

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

protected:

	void SpawnEnemy();

	UFUNCTION()
	void OnEnemyDied();

	void SpawnerDepleted();

public:

	// ~begin ICombatActivatable interface
	virtual void ToggleInteraction(AActor* ActivationInstigator) override;
	virtual void ActivateInteraction(AActor* ActivationInstigator) override;
	virtual void DeactivateInteraction(AActor* ActivationInstigator) override;
	// ~end ICombatActivatable interface
};
