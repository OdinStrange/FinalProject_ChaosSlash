// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultGameModeBase.h"
#include "ChaosSlashCharacter.h"
#include "Engine/Engine.h"

ADefaultGameModeBase::ADefaultGameModeBase()
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("ADefaultGameModeBase Call"));
	////�� ������ ȭ���� -1, 3.0f ��ǥ�� ���� �۾��� ����� �ؽ�Ʈ�� ��� ����� ��
	static ConstructorHelpers::FClassFinder<AChaosSlashCharacter>
		PlayerCharacter(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.BP_ThirdPersonCharacter_C"));
	//�����ڿ��� BP_ThirdPersonCharacter ��������Ʈ Ŭ������ ã�Ƽ� PlayerCharacter��� ������ ��´�.
	
	if (PlayerCharacter.Class)//����ó��
	{
		DefaultPawnClass = PlayerCharacter.Class;//�⺻ �� Ŭ������ �÷��̾�ĳ���� Ŭ������ �����϶�.
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("PlayerCharacter.Class InB"));
	}
}
