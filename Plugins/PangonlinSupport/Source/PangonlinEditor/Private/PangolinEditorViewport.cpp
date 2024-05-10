// Fill out your copyright notice in the Description page of Project Settings.


#include "PangolinEditorViewport.h"

void SPangolinEditorViewport::Construct(const FArguments& InArgs)
{
	Terrian = InArgs._Terrian.Get();
	SEditorViewport::Construct(SEditorViewport::FArguments());
	
}

TSharedRef<class SEditorViewport> SPangolinEditorViewport::GetViewportWidget()
{
	return SharedThis(this);
}

TSharedPtr<FExtender> SPangolinEditorViewport::GetExtenders() const
{
	return MakeShareable<FExtender>(new FExtender());
}


TSharedRef<FEditorViewportClient> SPangolinEditorViewport::MakeEditorViewportClient()
{
	ViewportClient = MakeShareable(new FPangolinEditorViewportClient(Terrian, SharedThis(this), PreviewScene.ToSharedRef()));
	ViewportClient->OnAssetChanged();
	return ViewportClient.ToSharedRef();
}


ATerrainActor* SPangolinEditorViewport::GetTerrianActor() const
{
	return ViewportClient->GetTerrianActor();
}

void SPangolinEditorViewport::PostTerrainInfoUpdated()
{
	ViewportClient->OnAssetChanged();
}
