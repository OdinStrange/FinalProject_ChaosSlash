// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/EnemyBase.h"
#include "Monster/EnemyAIController.h"
#include "Player/PlayerCharacterBase.h"
#include "HUDUserWidget.h"
#include "PotionPickup.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	InitializeAIController();

	// 적이 카메라와 플레이어 사이를 지나도 SpringArm이 반응하지 않도록
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackMontageFinder(
		TEXT("/Game/ChaosSlash/Enemy/Attack_PrimaryA_Montage.Attack_PrimaryA_Montage"));
	if (AttackMontageFinder.Succeeded())
	{
		AttackMontage = AttackMontageFinder.Object;
	}
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	InitializeMeshes();
	InitializeAnimInstance();
	InitializeSize();
	InitializeHp();
	InitializeDamage();

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (APlayerCharacterBase* PlayerChar = Cast<APlayerCharacterBase>(PC->GetPawn()))
		{
			if (UHUDUserWidget* HUD = Cast<UHUDUserWidget>(PlayerChar->HUDWidget))
			{
				if (HUD->EnemyHPBar)
				{
					HUD->EnemyHPBar->SetVisibility(ESlateVisibility::Visible);
					HUD->EnemyHPBar->SetPercent(1.f);
				}
			}
		}
	}
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyBase::InitializeMeshes()
{
	if (USkeletalMesh** FoundMesh = EnemyMeshes.Find(CurrentEnemyType))
	{
		GetMesh()->SetSkeletalMesh(*FoundMesh);
	}
	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
}

void AEnemyBase::InitializeAnimInstance()
{
	if (TSubclassOf<UAnimInstance>* FoundAnim = EnemyAnimBlueprints.Find(CurrentEnemyType))
	{
		GetMesh()->SetAnimInstanceClass(*FoundAnim);
	}
}

void AEnemyBase::InitializeSize()
{
	if (CurrentEnemyType == EEnemyType::NORMALENEMY)
	{
		EnemySize = FVector(1.1f, 1.1f, 1.1f);
		SetActorScale3D(EnemySize);
	}
	else if (CurrentEnemyType == EEnemyType::MAGICENEMY)
	{
		EnemySize = FVector(1.5f, 1.5f, 1.5f);
		SetActorScale3D(EnemySize);
	}
}

void AEnemyBase::InitializeHp()
{
	if (CurrentEnemyType == EEnemyType::NORMALENEMY)
	{
		MaxEnemyHp = 100;
	}
	else if (CurrentEnemyType == EEnemyType::MAGICENEMY)
	{
		MaxEnemyHp = 200;
	}
	CurrentEnemyHp = MaxEnemyHp; //시작 시 현재 HP를 최대 HP로 초기화
}

void AEnemyBase::Attack()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !AttackMontage) return;
	if (AnimInstance->Montage_IsPlaying(AttackMontage)) return;

	AnimInstance->Montage_Play(AttackMontage);
}

float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	//부모(AActor)의 TakeDamage를 먼저 실행해 엔진 내부 처리를 보장
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	//CurrentEnemyHp에서 데미지를 빼되 0~MaxEnemyHp 범위를 벗어나지 않게 고정
	CurrentEnemyHp = FMath::Clamp(CurrentEnemyHp - ActualDamage, 0.f, MaxEnemyHp);

	// 피격 사운드 랜덤 재생
	if (HitSounds.Num() > 0)
	{
		int32 Index = FMath::RandRange(0, HitSounds.Num() - 1);
		UGameplayStatics::PlaySoundAtLocation(this, HitSounds[Index], GetActorLocation());
	}

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (APlayerCharacterBase* PlayerChar = Cast<APlayerCharacterBase>(PC->GetPawn()))
		{
			if (UHUDUserWidget* HUD = Cast<UHUDUserWidget>(PlayerChar->HUDWidget))
			{
				if (HUD->EnemyHPBar)
				{
					HUD->EnemyHPBar->SetVisibility(ESlateVisibility::Visible);
					HUD->EnemyHPBar->SetPercent(CurrentEnemyHp / MaxEnemyHp);
				}
			}
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
		FString::Printf(TEXT("[Enemy] 데미지: %.0f | HP: %.0f / %.0f"), ActualDamage, CurrentEnemyHp, MaxEnemyHp));

	if (CurrentEnemyHp <= 0.f)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, TEXT("[Enemy] 사망 → Destroy"));

		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			if (APlayerCharacterBase* PlayerChar = Cast<APlayerCharacterBase>(PC->GetPawn()))
			{
				if (UHUDUserWidget* HUD = Cast<UHUDUserWidget>(PlayerChar->HUDWidget))
				{
					if (HUD->EnemyHPBar)
						HUD->EnemyHPBar->SetVisibility(ESlateVisibility::Hidden);
				}
			}
		}

		APlayerCharacterBase* PlayerChar = Cast<APlayerCharacterBase>(DamageCauser);
		if (PlayerChar)
		{
			PlayerChar->AddKill();
		}

		TArray<AActor*> ExistingPotions;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APotionPickup::StaticClass(), ExistingPotions);
		if (DropPickupClass && FMath::FRand() <= DropChance && ExistingPotions.Num() < 3)
		{
			FVector SpawnLocation = GetActorLocation();
			SpawnLocation.Z += 50.0f;
			GetWorld()->SpawnActor<APotionPickup>(DropPickupClass, SpawnLocation, FRotator::ZeroRotator);
		}

		OnEnemyDied.Broadcast();
		Destroy();
	}

	return ActualDamage; //실제로 적용된 데미지 값을 반환
}

void AEnemyBase::InitializeDamage()
{
	if (CurrentEnemyType == EEnemyType::NORMALENEMY)
	{
		EnemyDamage = 10;
	}
	else if (CurrentEnemyType == EEnemyType::MAGICENEMY)
	{
		EnemyDamage = 20;
	}
}

void AEnemyBase::InitializeAIController()
{
	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

