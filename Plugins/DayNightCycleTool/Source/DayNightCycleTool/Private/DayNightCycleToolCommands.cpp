// Copyright Epic Games, Inc. All Rights Reserved.

#include "DayNightCycleToolCommands.h"

#define LOCTEXT_NAMESPACE "FDayNightCycleToolModule"

void FDayNightCycleToolCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "DayNightCycleTool", "Bring up DayNightCycleTool window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
