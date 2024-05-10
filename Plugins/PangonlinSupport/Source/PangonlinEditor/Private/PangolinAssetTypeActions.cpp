
// Fill out your copyright notice in the Description page of Project Settings.


#include "PangolinAssetTypeActions.h"
#include "PangolinTerrain.h"
#include "PangolinEditorToolkit.h"


FPangolinAssetTypeActions::FPangolinAssetTypeActions
(EAssetTypeCategories::Type InAssetTypeCategory, const FText& InAssetTypeName = FText::FromString("Pangolin"), const FColor& InAssetTypeColor = FColor(143, 78, 53))
{
	AssetTypeCategory = (InAssetTypeCategory);
		AssetTypeName = (InAssetTypeName);
		AssetTypeColor = (InAssetTypeColor);
}

UClass* FPangolinAssetTypeActions::GetSupportedClass() const
{
	return UPangolinTerrain::StaticClass();
}

FText FPangolinAssetTypeActions::GetName() const
{
	return AssetTypeName;
}

FColor FPangolinAssetTypeActions::GetTypeColor() const
{
	return AssetTypeColor;
}

uint32 FPangolinAssetTypeActions::GetCategories()
{
	return AssetTypeCategory;
}

void FPangolinAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	MakeShared<FPangolinEditorToolkit>()->InitEditor(InObjects);
}

