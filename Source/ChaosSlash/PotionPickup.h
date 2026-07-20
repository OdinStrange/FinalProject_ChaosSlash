#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PotionPickup.generated.h"

class USphereComponent;

UCLASS(abstract)
class CHAOSSLASH_API APotionPickup : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	USphereComponent* Sphere;

public:

	APotionPickup();

protected:

	UFUNCTION()
	void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	/** 블루프린트에서 이펙트 재생 및 DestroyActor 호출 */
	UFUNCTION(BlueprintImplementableEvent, Category="Pickup", meta=(DisplayName="On Picked Up"))
	void BP_OnPickedUp();
};
