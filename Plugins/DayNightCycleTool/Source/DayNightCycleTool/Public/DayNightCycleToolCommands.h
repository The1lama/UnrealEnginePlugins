// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Framework/Commands/Commands.h"
#include "DayNightCycleToolStyle.h"

class FDayNightCycleToolCommands : public TCommands<FDayNightCycleToolCommands>
{
public:

	FDayNightCycleToolCommands()
		: TCommands<FDayNightCycleToolCommands>(TEXT("DayNightCycleTool"), NSLOCTEXT("Contexts", "DayNightCycleTool", "DayNightCycleTool Plugin"), NAME_None, FDayNightCycleToolStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};