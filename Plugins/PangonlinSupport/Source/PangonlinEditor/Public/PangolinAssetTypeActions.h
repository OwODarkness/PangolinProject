// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
/**
 * 
 */
class  FPangolinAssetTypeActions : public FAssetTypeActions_Base
{
public:
	FPangolinAssetTypeActions(EAssetTypeCategories::Type AssetTypeCategory,const FText& AssetTypeName,const FColor& AssetTypeColor);

	UClass* GetSupportedClass() const override;

	FText GetName() const override;

	FColor GetTypeColor() const override;

	uint32 GetCategories() override;

	void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor) override;

private:
	EAssetTypeCategories::Type AssetTypeCategory;
	FColor AssetTypeColor;
	FText AssetTypeName;
};
