// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiPlayerAppGameMode.h"
#include "MultiPlayerAppCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMultiPlayerAppGameMode::AMultiPlayerAppGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter_2"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
