// Copyright Epic Games, Inc. All Rights Reserved.

#include "PangolinEditorCommands.h"

#define LOCTEXT_NAMESPACE "FPangonlinEditorModule"

void FPangolinEditorCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "PangonlinSupport", "Bring up PangonlinSupport window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
