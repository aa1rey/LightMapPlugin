// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "LightMapBakerStyle.h"

class FLightMapBakerCommands : public TCommands<FLightMapBakerCommands>
{
public:

	FLightMapBakerCommands()
		: TCommands<FLightMapBakerCommands>(TEXT("LightMapBaker"), NSLOCTEXT("Contexts", "LightMapBaker", "LightMapBaker Plugin"), NAME_None, FLightMapBakerStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
