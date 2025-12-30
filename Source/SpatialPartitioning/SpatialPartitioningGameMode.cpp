// Copyright Epic Games, Inc. All Rights Reserved.

#include "SpatialPartitioningGameMode.h"
#include "SpatialPartitioningCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASpatialPartitioningGameMode::ASpatialPartitioningGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
