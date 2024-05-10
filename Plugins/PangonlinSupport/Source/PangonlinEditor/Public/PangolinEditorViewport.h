// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SEditorViewport.h"
#include "SCommonEditorViewportToolbarBase.h"
#include "AdvancedPreviewScene.h"
#include "PangolinEditorViewportClient.h"



class UPangolinTerrain;

/**
 * 
 */
class SPangolinEditorViewport : 
public SEditorViewport,
public FGCObject,
public ICommonEditorViewportToolbarInfoProvider
{
public:
	SLATE_BEGIN_ARGS(SPangolinEditorViewport){}

	SLATE_ATTRIBUTE(UPangolinTerrain*, Terrian)


	SLATE_END_ARGS()

	SPangolinEditorViewport():PreviewScene(MakeShareable(new FAdvancedPreviewScene(FAdvancedPreviewScene::ConstructionValues()))) {}

	void Construct(const FArguments& InArgs);

	// Get the viewport widget
	TSharedRef<class SEditorViewport> GetViewportWidget() override;

	TSharedPtr<FExtender> GetExtenders() const override;

	void OnFloatingButtonClicked() override{};

	void AddReferencedObjects(FReferenceCollector& Collector) override{}

	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;

	ATerrainActor* GetTerrianActor() const;

	UFUNCTION()
		void PostTerrainInfoUpdated();
private:
	TSharedPtr<FAdvancedPreviewScene> PreviewScene;

	TSharedPtr<FPangolinEditorViewportClient> ViewportClient;

	UPangolinTerrain* Terrian;


};
