#include "PrintSettings.h"

#include "DebugPrintSettings.h"
#include "ISettingsModule.h"
#include "UObject/UObjectGlobals.h"

#define LOCTEXT_NAMESPACE "FPrintSettingsModule"

void FPrintSettingsModule::StartupModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings(
			"Project",
			"Development",
			"Debug Print",
			LOCTEXT("DebugPrintSettingsName", "Debug Print Settings"),
			LOCTEXT("DebugPrintSettingsDesc", "Settings for the Debug Print plugin"),
			GetMutableDefault<UDebugPrintSettings>()
		);
	}
}

void FPrintSettingsModule::ShutdownModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Development", "Debug Print");
	}
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FPrintSettingsModule, PrintSettings)