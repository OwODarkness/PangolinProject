// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "HeightMapWidget.h"
#include "TerrianNoisePlotWidget.h"
#include "PangolinEditorViewport.h"

/**
 * 
 */
class UPangolinTerrain;

class FPangolinEditorToolkit: public FAssetEditorToolkit
{
public:

	void InitEditor(const TArray<UObject*>  InObjects);

	void RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;

	void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;

	FName GetToolkitFName() const override{return "PangonlinTerrianEditor";}
	FText GetBaseToolkitName() const override{return INVTEXT("Pangolin Terrian Editor");}

	FString GetWorldCentricTabPrefix() const override{return "Pangolin Terrian";}

	FLinearColor GetWorldCentricTabColorScale() const override
	{
		return FLinearColor::Yellow;
	}
	UFUNCTION()
	FReply OnTerrianGenerateButtonClicked();

	UFUNCTION() 
	void SetGrayScaleNoiseTexture(UTexture2D* Texture);
	UFUNCTION()
	void SetColorScaleNoiseTexture(UTexture2D* Texture);
	UFUNCTION()
	FReply OnExportButtonClicked();
	UFUNCTION()
	void OnNoisePlotUpdateEvent(int32 Index, bool Flag);

private:
	UPangolinTerrain* Terrian;

	TSharedPtr<SHeightMapWidget> GrayScaleNoiseWidget;
	TSharedPtr<SHeightMapWidget> ColorScaleNoiseWidget;
	TSharedPtr<STerrianNoisePlotWidget> NoisePlotWidget;
	TSharedPtr<SPangolinEditorViewport> TerrianViewport;
};
