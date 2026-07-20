#include "CombatPotionPickup.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CombatDamageable.h"

ACombatPotionPickup::ACombatPotionPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Sphere->SetupAttachment(RootComponent);
	Sphere->SetSphereRadius(60.0f);
	Sphere->SetCollisionObjectType(ECC_WorldDynamic);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	OnActorBeginOverlap.AddDynamic(this, &ACombatPotionPickup::BeginOverlap);
}

void ACombatPotionPickup::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	ACharacter* OverlappedCharacter = Cast<ACharacter>(OtherActor);
	if (OverlappedCharacter && OverlappedCharacter->IsPlayerControlled())
	{
		ICombatDamageable* Damageable = Cast<ICombatDamageable>(OtherActor);
		if (Damageable)
		{
			Damageable->ApplyHealing(HealAmount, this);
			SetActorEnableCollision(false);
			BP_OnPickedUp();
		}
	}
}
