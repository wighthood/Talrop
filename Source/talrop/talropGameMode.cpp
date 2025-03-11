// Copyright Epic Games, Inc. All Rights Reserved.

#include "talropGameMode.h"
#include "talropCharacter.h"
#include "UObject/ConstructorHelpers.h"

AtalropGameMode::AtalropGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
