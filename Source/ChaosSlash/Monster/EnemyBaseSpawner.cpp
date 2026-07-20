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
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	SpawnCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("SpawnCapsule"));
	SpawnCapsule->SetupAttachment(RootComponent);
	SpawnCapsule->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));
	SpawnCapsule->SetCapsuleSize(35.0f, 90.0f);
	SpawnCapsule->SetCollisionProfileName(FName("NoCollision"));

	SpawnDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnDirection"));
	SpawnDirection->SetupAttachment(RootComponent);
}

void AEnemyBaseSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (bShouldSpawnEnemiesImmediately)
	{
		GetWorld()->GetTimerManager().SetTimer(SpawnTimer, this, &AEnemyBaseSpawner::SpawnEnemy, InitialSpawnDelay);
	}
}

void AEnemyBaseSpawner::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
	GetWorld()->GetTimerManager().ClearTimer(ClearCheckTimer);
}

void AEnemyBaseSpawner::SpawnEnemy()
{
	if (!IsValid(EnemyClass))
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AEnemyBase* SpawnedEnemy = GetWorld()->SpawnActor<AEnemyBase>(EnemyClass, SpawnCapsule->GetComponentTransform(), SpawnParams);
	if (SpawnedEnemy)
	{
		SpawnedEnemy->OnEnemyDied.AddDynamic(this, &AEnemyBaseSpawner::OnEnemyDied);
		LiveEnemyCount++;
	}

	--SpawnCount;
	if (SpawnCount <= 0)
	{
		bSpawnerExhausted = true;
		SpawnerDepleted();
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(SpawnTimer, this, &AEnemyBaseSpawner::SpawnEnemy, RespawnDelay, false);
}

void AEnemyBaseSpawner::OnEnemyDied()
{
	--LiveEnemyCount;

	// 적 사망 직후엔 아직 액터가 Destroy 전이므로 0.1초 후에 체크
	GetWorld()->GetTimerManager().SetTimer(ClearCheckTimer, this, &AEnemyBaseSpawner::CheckClearCondition, 0.1f, false);
}

void AEnemyBaseSpawner::CheckClearCondition()
{
	// 월드에 살아있는 적이 남아있으면 클리어 아님
	TArray<AActor*> RemainingEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyBase::StaticClass(), RemainingEnemies);
	if (RemainingEnemies.Num() > 0) return;

	// 소진되지 않은 스포너가 있으면 클리어 아님
	TArray<AActor*> AllSpawners;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyBaseSpawner::StaticClass(), AllSpawners);
	for (AActor* SpawnerActor : AllSpawners)
	{
		AEnemyBaseSpawner* Spawner = Cast<AEnemyBaseSpawner>(SpawnerActor);
		if (Spawner && !Spawner->IsExhausted()) return;
	}

	APlayerCharacterBase* Player = Cast<APlayerCharacterBase>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Player)
	{
		Player->ShowGameClearUI();
	}
}

void AEnemyBaseSpawner::SpawnerDepleted()
{
	for (AActor* Actor : ActorsToActivateWhenDepleted)
	{
		if (ICombatActivatable* Activatable = Cast<ICombatActivatable>(Actor))
		{
			Activatable->ActivateInteraction(this);
		}
	}
}

void AEnemyBaseSpawner::ToggleInteraction(AActor* ActivationInstigator)
{
	// stub
}

void AEnemyBaseSpawner::ActivateInteraction(AActor* ActivationInstigator)
{
	if (bHasBeenActivated || bShouldSpawnEnemiesImmediately)
	{
		return;
	}

	bHasBeenActivated = true;
	SpawnEnemy();
}

void AEnemyBaseSpawner::DeactivateInteraction(AActor* ActivationInstigator)
{
	// stub
}
