// Copyright Epic Games, Inc. All Rights Reserved.

#include "LightMapBakerCommands.h"

#define LOCTEXT_NAMESPACE "FLightMapBakerModule"

void FLightMapBakerCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "LightMapBaker", "Execute LightMapBaker action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
