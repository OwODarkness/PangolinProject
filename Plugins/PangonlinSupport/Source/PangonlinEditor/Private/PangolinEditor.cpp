// Copyright Epic Games, Inc. All Rights Reserved.

#include "PangolinEditor.h"
#include "PangolinEditorStyle.h"
#include "PangolinEditorCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"


static const FName PangonlinSupportTabName("PangonlinSupport");

#define LOCTEXT_NAMESPACE "FPangonlinSupportModule"

void FPangolinEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FPangolinEditorStyle::Initialize();
	FPangolinEditorStyle::ReloadTextures();

	FPangolinEditorCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FPangolinEditorCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FPangolinEditorModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FPangolinEditorModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(PangonlinSupportTabName, FOnSpawnTab::CreateRaw(this, &FPangolinEditorModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FPangonlinSupportTabTitle", "PangonlinSupport"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	PangolinCategory = AssetTools.RegisterAdvancedAssetCategory(FName("Pangolin"), FText::FromString("Pangolin"));

	PangonlinAssetTypeAction = MakeShared<FPangolinAssetTypeActions>(PangolinCategory);
	AssetTools.RegisterAssetTypeActions(PangonlinAssetTypeAction.ToSharedRef());
}

void FPangolinEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FPangolinEditorStyle::Shutdown();

	FPangolinEditorCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(PangonlinSupportTabName);

}

TSharedRef<SDockTab> FPangolinEditorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FPangonlinSupportModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("PangonlinSupport.cpp"))
		);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(WidgetText)
			]
		];
}

void FPangolinEditorModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(PangonlinSupportTabName);
}

void FPangolinEditorModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FPangolinEditorCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(
					FToolMenuEntry::InitToolBarButton(
					FPangolinEditorCommands::Get().OpenPluginWindow,
					FText::FromString("Pangolin"),
					FText::FromString("Pangolin Plugin"),
					FSlateIcon(FPangolinEditorStyle::GetStyleSetName(), "PangonlinSupport.PangonlinIcon")
					));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPangolinEditorModule, PangonlinEditor)