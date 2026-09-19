// Copyright Epic Games, Inc. All Rights Reserved.

#include "DayNightCycleTool.h"
#include "DayNightCycleToolStyle.h"
#include "DayNightCycleToolCommands.h"
#include "SSlateDayNightWindowMenu.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

static const FName DayNightCycleToolTabName("DayNightCycleTool");

#define LOCTEXT_NAMESPACE "FDayNightCycleToolModule"

void FDayNightCycleToolModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FDayNightCycleToolStyle::Initialize();
	FDayNightCycleToolStyle::ReloadTextures();

	FDayNightCycleToolCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FDayNightCycleToolCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FDayNightCycleToolModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FDayNightCycleToolModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(DayNightCycleToolTabName, FOnSpawnTab::CreateRaw(this, &FDayNightCycleToolModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FDayNightCycleToolTabTitle", "DayNightCycleTool"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FDayNightCycleToolModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FDayNightCycleToolStyle::Shutdown();

	FDayNightCycleToolCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(DayNightCycleToolTabName);
}

TSharedRef<SDockTab> FDayNightCycleToolModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "{0} {1} "),
		FText::FromString(TEXT("Hello, ")),
		FText::FromString(TEXT("World!"))
		);
	
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SSlateDayNightWindowMenu)
		];
}

void FDayNightCycleToolModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(DayNightCycleToolTabName);
}

void FDayNightCycleToolModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FDayNightCycleToolCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FDayNightCycleToolCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDayNightCycleToolModule, DayNightCycleTool)