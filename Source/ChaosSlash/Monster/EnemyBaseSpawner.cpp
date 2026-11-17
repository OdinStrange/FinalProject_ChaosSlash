#include "EnemyBaseSpawner.h"
#include "EnemyBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"
#include "TimerManager.h"
#include "Variant_Combat/Interfaces/CombatActivatable.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayerCharacterBase.h"

AEnemyBaseSpawner::AEnemyBaseSpawner()
{
	// 스포너는 타이머로만 동작하므로 매 프레임 갱신하지 않습니다.
	PrimaryActorTick.bCanEverTick = false;

	// 스포너의 기준 루트 컴포넌트를 생성합니다.
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// 적이 생성될 위치를 나타내는 캡슐 컴포넌트입니다.
	SpawnCapsule =
		CreateDefaultSubobject<UCapsuleComponent>(TEXT("SpawnCapsule"));

	SpawnCapsule->SetupAttachment(RootComponent);
	SpawnCapsule->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));
	SpawnCapsule->SetCapsuleSize(35.0f, 90.0f);

	// 스포너 위치가 다른 액터와 충돌하지 않도록 설정합니다.
	SpawnCapsule->SetCollisionProfileName(FName("NoCollision"));

	// 에디터에서 스폰 방향을 확인하기 위한 화살표입니다.
	SpawnDirection =
		CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnDirection"));

	SpawnDirection->SetupAttachment(RootComponent);
}

void AEnemyBaseSpawner::BeginPlay()
{
	Super::BeginPlay();

	// 모든 스폰 항목의 생성 수량을 합산합니다.
	RemainingSpawnCount = 0;

	for (const FEnemySpawnEntry& SpawnEntry : EnemySpawnEntries)
	{
		if (IsValid(SpawnEntry.EnemyClass) &&
			SpawnEntry.SpawnCount > 0)
		{
			RemainingSpawnCount += SpawnEntry.SpawnCount;
		}
	}

	// 처음 사용할 스폰 항목을 찾습니다.
	CurrentSpawnEntryIndex = 0;
	CurrentEntryRemainingCount = 0;

	while (CurrentSpawnEntryIndex < EnemySpawnEntries.Num())
	{
		const FEnemySpawnEntry& SpawnEntry =
			EnemySpawnEntries[CurrentSpawnEntryIndex];

		// 클래스가 지정되지 않았거나 수량이 0이면 건너뜁니다.
		if (IsValid(SpawnEntry.EnemyClass) &&
			SpawnEntry.SpawnCount > 0)
		{
			CurrentEntryRemainingCount = SpawnEntry.SpawnCount;
			break;
		}

		++CurrentSpawnEntryIndex;
	}

	// 유효한 스폰 항목이 없으면 스포너를 종료합니다.
	if (RemainingSpawnCount <= 0 ||
		CurrentSpawnEntryIndex >= EnemySpawnEntries.Num())
	{
		bSpawnerExhausted = true;
		SpawnerDepleted();
		return;
	}

	// 게임 시작 즉시 생성하도록 설정된 경우 첫 적을 예약합니다.
	if (bShouldSpawnEnemiesImmediately)
	{
		GetWorld()->GetTimerManager().SetTimer(
			SpawnTimer,
			this,
			&AEnemyBaseSpawner::SpawnEnemy,
			InitialSpawnDelay,
			false);
	}
}

void AEnemyBaseSpawner::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// 액터가 제거될 때 예약된 타이머를 모두 정리합니다.
	GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
	GetWorld()->GetTimerManager().ClearTimer(ClearCheckTimer);
}

void AEnemyBaseSpawner::SpawnEnemy()
{
	// 현재 항목의 생성 수량이 끝났으면 다음 항목으로 이동합니다.
	while (CurrentSpawnEntryIndex < EnemySpawnEntries.Num() &&
		CurrentEntryRemainingCount <= 0)
	{
		++CurrentSpawnEntryIndex;

		if (CurrentSpawnEntryIndex < EnemySpawnEntries.Num())
		{
			CurrentEntryRemainingCount =
				EnemySpawnEntries[CurrentSpawnEntryIndex].SpawnCount;
		}
	}

	// 모든 스폰 항목을 처리했으면 스포너를 종료합니다.
	if (CurrentSpawnEntryIndex >= EnemySpawnEntries.Num() ||
		RemainingSpawnCount <= 0)
	{
		bSpawnerExhausted = true;
		SpawnerDepleted();
		return;
	}

	// 현재 생성할 적 정보를 가져옵니다.
	const FEnemySpawnEntry& CurrentSpawnEntry =
		EnemySpawnEntries[CurrentSpawnEntryIndex];

	// 적 블루프린트가 비어 있으면 현재 항목을 건너뜁니다.
	if (!IsValid(CurrentSpawnEntry.EnemyClass))
	{
		CurrentEntryRemainingCount = 0;
		SpawnEnemy();
		return;
	}

	const FTransform SpawnTransform =
		SpawnCapsule->GetComponentTransform();

	// BeginPlay() 전에 적 타입을 지정하기 위해 지연 생성합니다.
	AEnemyBase* SpawnedEnemy =
		GetWorld()->SpawnActorDeferred<AEnemyBase>(
			CurrentSpawnEntry.EnemyClass,
			SpawnTransform,
			this,
			nullptr,
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

	if (!SpawnedEnemy)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[EnemySpawner] 적 액터 생성에 실패했습니다."));

		return;
	}

	// BeginPlay()에서 메시, 애니메이션, 체력을 올바르게 초기화할 수 있도록
	// 액터가 완전히 등록되기 전에 적 종류를 설정합니다.
	SpawnedEnemy->CurrentEnemyType =
		CurrentSpawnEntry.EnemyType;

	// 타입 설정이 완료된 후 액터를 월드에 등록합니다.
	SpawnedEnemy->FinishSpawning(SpawnTransform);

	// 적이 사망했을 때 스포너가 알 수 있도록 이벤트를 연결합니다.
	SpawnedEnemy->OnEnemyDied.AddDynamic(
		this,
		&AEnemyBaseSpawner::OnEnemyDied);

	// 현재 스폰 항목과 전체 남은 수량을 감소시킵니다.
	--CurrentEntryRemainingCount;
	--RemainingSpawnCount;
	++LiveEnemyCount;

	UE_LOG(
		LogTemp,
		Display,
		TEXT("[EnemySpawner] %s 생성 완료. 남은 적 수: %d"),
		CurrentSpawnEntry.EnemyType == EEnemyType::NORMALENEMY
			? TEXT("NORMALENEMY")
			: TEXT("MAGICENEMY"),
		RemainingSpawnCount);

	// 모든 적을 생성했다면 스포너를 소진 상태로 변경합니다.
	if (RemainingSpawnCount <= 0)
	{
		bSpawnerExhausted = true;
		SpawnerDepleted();
		return;
	}

	// 다음 적은 RespawnDelay초 후에 생성합니다.
	GetWorld()->GetTimerManager().SetTimer(
		SpawnTimer,
		this,
		&AEnemyBaseSpawner::SpawnEnemy,
		RespawnDelay,
		false);
}

void AEnemyBaseSpawner::OnEnemyDied()
{
	// 사망한 적의 수를 감소시킵니다.
	LiveEnemyCount = FMath::Max(0, LiveEnemyCount - 1);

	// 액터가 완전히 제거된 뒤 클리어 조건을 확인합니다.
	GetWorld()->GetTimerManager().SetTimer(
		ClearCheckTimer,
		this,
		&AEnemyBaseSpawner::CheckClearCondition,
		0.1f,
		false);
}

void AEnemyBaseSpawner::CheckClearCondition()
{
	// 아직 살아 있는 적이 있으면 클리어가 아닙니다.
	TArray<AActor*> RemainingEnemies;

	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		AEnemyBase::StaticClass(),
		RemainingEnemies);

	if (RemainingEnemies.Num() > 0)
	{
		return;
	}

	// 아직 소진되지 않은 스포너가 있으면 클리어가 아닙니다.
	TArray<AActor*> AllSpawners;

	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		AEnemyBaseSpawner::StaticClass(),
		AllSpawners);

	for (AActor* SpawnerActor : AllSpawners)
	{
		AEnemyBaseSpawner* Spawner =
			Cast<AEnemyBaseSpawner>(SpawnerActor);

		if (Spawner && !Spawner->IsExhausted())
		{
			return;
		}
	}

	// 모든 적과 스포너가 처리되면 게임 클리어 UI를 표시합니다.
	APlayerCharacterBase* Player =
		Cast<APlayerCharacterBase>(
			UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (Player)
	{
		Player->ShowGameClearUI();
	}
}

void AEnemyBaseSpawner::SpawnerDepleted()
{
	// 스포너가 모든 적을 생성하면 연결된 액터를 활성화합니다.
	for (AActor* Actor : ActorsToActivateWhenDepleted)
	{
		if (ICombatActivatable* Activatable =
			Cast<ICombatActivatable>(Actor))
		{
			Activatable->ActivateInteraction(this);
		}
	}
}

void AEnemyBaseSpawner::ToggleInteraction(
	AActor* ActivationInstigator)
{
	// 현재 스포너에서는 토글 기능을 사용하지 않습니다.
}

void AEnemyBaseSpawner::ActivateInteraction(
	AActor* ActivationInstigator)
{
	// 이미 활성화되었거나 즉시 생성 모드이면 중복 실행하지 않습니다.
	if (bHasBeenActivated || bShouldSpawnEnemiesImmediately)
	{
		return;
	}

	bHasBeenActivated = true;

	// 첫 번째 적을 생성합니다.
	// 이후 적은 RespawnDelay 간격으로 생성됩니다.
	SpawnEnemy();
}

void AEnemyBaseSpawner::DeactivateInteraction(
	AActor* ActivationInstigator)
{
	// 현재 스포너에서는 비활성화 기능을 사용하지 않습니다.
}