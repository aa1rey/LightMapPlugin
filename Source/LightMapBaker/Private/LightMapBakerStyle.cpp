// Copyright Epic Games, Inc. All Rights Reserved.

#include "LightMapBakerStyle.h"
#include "LightMapBaker.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FLightMapBakerStyle::StyleInstance = nullptr;

void FLightMapBakerStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FLightMapBakerStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FLightMapBakerStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("LightMapBakerStyle"));
	return StyleSetName;
}


const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FLightMapBakerStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("LightMapBakerStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("LightMapBaker")->GetBaseDir() / TEXT("Resources"));

	Style->Set("LightMapBaker.PluginAction", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));
	return Style;
}

void FLightMapBakerStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FLightMapBakerStyle::Get()
{
	return *StyleInstance;
}
