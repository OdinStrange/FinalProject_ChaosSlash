#include "PotionPickup.h"
#include "Player/PlayerCharacterBase.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

APotionPickup::APotionPickup()
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

	OnActorBeginOverlap.AddDynamic(this, &APotionPickup::BeginOverlap);
}

void APotionPickup::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	APlayerCharacterBase* Player = Cast<APlayerCharacterBase>(OtherActor);
	if (Player && Player->IsPlayerControlled())
	{
		if (Player->AddPotion(1))
		{
			SetActorEnableCollision(false);
			BP_OnPickedUp();
		}
	}
}
