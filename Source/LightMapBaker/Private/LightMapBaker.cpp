// Copyright Epic Games, Inc. All Rights Reserved.

#include "LightMapBaker.h"
#include "LightMapBakerStyle.h"
#include "LightMapBakerCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

static const FName LightMapBakerTabName("LightMapBaker");

#define LOCTEXT_NAMESPACE "FLightMapBakerModule"

void FLightMapBakerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FLightMapBakerStyle::Initialize();
	FLightMapBakerStyle::ReloadTextures();

	FLightMapBakerCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FLightMapBakerCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FLightMapBakerModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FLightMapBakerModule::RegisterMenus));
}

void FLightMapBakerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FLightMapBakerStyle::Shutdown();

	FLightMapBakerCommands::Unregister();
}

void FLightMapBakerModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here

	if (UWorld* EditorWorld = GEditor->GetEditorWorldContext().World())
	{
		FText DialogText = FText::Format(
			LOCTEXT("PluginButtonDialogText", "Editor world is {0}"),
			FText::FromString(EditorWorld->GetName())
		);
		FMessageDialog::Open(EAppMsgType::Ok, DialogText);
		FMessageDialog::Debugf(DialogText);
	}

	
}

void FLightMapBakerModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FLightMapBakerCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FLightMapBakerCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLightMapBakerModule, LightMapBaker)