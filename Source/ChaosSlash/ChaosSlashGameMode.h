// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ChaosSlashGameMode.generated.h"

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class AChaosSlashGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	/** Constructor */
	AChaosSlashGameMode();
};
