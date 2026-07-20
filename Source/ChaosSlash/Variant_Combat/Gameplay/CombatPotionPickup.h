#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatPotionPickup.generated.h"

class USphereComponent;

UCLASS(abstract)
class ACombatPotionPickup : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	USphereComponent* Sphere;

public:

	ACombatPotionPickup();

protected:

	/** Amount of HP to restore on pickup */
	UPROPERTY(EditAnywhere, Category="Pickup", meta=(ClampMin=0, ClampMax=100))
	float HealAmount = 1.0f;

	UFUNCTION()
	void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	/** Implemented in Blueprint to play effects and destroy the actor */
	UFUNCTION(BlueprintImplementableEvent, Category="Pickup", meta=(DisplayName="On Picked Up"))
	void BP_OnPickedUp();
};
