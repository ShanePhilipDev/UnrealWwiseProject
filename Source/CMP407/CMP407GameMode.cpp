// Copyright Epic Games, Inc. All Rights Reserved.

#include "CMP407GameMode.h"
#include "CMP407Character.h"
#include "UObject/ConstructorHelpers.h"
#include "AkGameplayStatics.h"

ACMP407GameMode::ACMP407GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
