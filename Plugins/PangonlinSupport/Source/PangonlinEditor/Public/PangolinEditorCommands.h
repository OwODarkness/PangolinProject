// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "PangolinEditorStyle.h"

class FPangolinEditorCommands : public TCommands<FPangolinEditorCommands>
{
public:

	FPangolinEditorCommands()
		: TCommands<FPangolinEditorCommands>(TEXT("PangonlinSupport"), NSLOCTEXT("Contexts", "PangonlinSupport", "PangonlinSupport Plugin"), NAME_None, FPangolinEditorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};