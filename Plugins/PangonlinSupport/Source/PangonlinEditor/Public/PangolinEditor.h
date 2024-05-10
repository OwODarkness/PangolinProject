// Copyright Epic Games, Inc. All Rights Reserved.
/*****************************************************************//**
 * \file   PangonlinSupport.h
 * \brief  穿山甲地形生成插件
 * 
 * \author OwODarkness	wjw
 * \date   February 2024
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "AssetTypeCategories.h"
#include "PangolinAssetTypeActions.h"

class FToolBarBuilder;
class FMenuBuilder;

class FPangolinEditorModule : public IModuleInterface
{
public:
	EAssetTypeCategories::Type PangolinCategory = EAssetTypeCategories::None;
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();
	
private:

	void RegisterMenus();

	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

private:
	TSharedPtr<class FUICommandList> PluginCommands;

	TSharedPtr<FPangolinAssetTypeActions> PangonlinAssetTypeAction;
};
