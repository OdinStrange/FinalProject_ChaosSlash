// Fill out your copyright notice in the Description page of Project Settings.


#include "ChaosSlash_AnimInstance.h"
#include "GameFrameWork/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


UChaosSlash_AnimInstance::UChaosSlash_AnimInstance()
{

}

void UChaosSlash_AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ACharacter>(GetOwningActor());

	if (Owner)
	{
		Movement = Owner->GetCharacterMovement();
	}
}

void UChaosSlash_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Movement)
	{
		Velocity = Movement->Velocity;
		MoveSpeed = Velocity.Size2D();
		isFalling = Movement->IsFalling();

	}
}
