// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackAnimNotify.h"
#include "Player/PlayerCharacterBase.h"
#include <Kismet/GameplayStatics.h>

void UAttackAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (AActor* Actor = MeshComp->GetOwner())
	{
		if (APlayerCharacterBase* Player = Cast<APlayerCharacterBase>(Actor))
		{
			if (Player->WeaponType == EWeaponType::SWORD)
			{
				if (Player->AttackSound)
				{
					UGameplayStatics::PlaySound2D(GetWorld(), Player->AttackSound);
				}
			}
			if (Player->WeaponType == EWeaponType::GREATSWORD)
			{
				if (Player->AttackSound)
				{
					UGameplayStatics::PlaySound2D(GetWorld(), Player->AttackSound);
				}
			}
			if (Player->WeaponType == EWeaponType::BLUNT)
			{
				if (Player->AttackSound)
				{
					UGameplayStatics::PlaySound2D(GetWorld(), Player->AttackSound);
				}
			}
			if (Player->WeaponType == EWeaponType::KATANA)
			{
				if (Player->AttackSound)
				{
					UGameplayStatics::PlaySound2D(GetWorld(), Player->AttackSound);
				}
			}
		}

	}
}
