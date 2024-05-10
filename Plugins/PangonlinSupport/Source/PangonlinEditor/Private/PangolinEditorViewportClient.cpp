// Fill out your copyright notice in the Description page of Project Settings.


#include "PangolinEditorViewportClient.h"
#include "PangolinTerrain.h"

FPangolinEditorViewportClient::FPangolinEditorViewportClient(class UPangolinTerrain* InTerrian, const TSharedRef<SEditorViewport>& InViewport, const TSharedRef<FAdvancedPreviewScene>& InPreviewScene)
	:FEditorViewportClient(nullptr, &InPreviewScene.Get(), StaticCastSharedRef<SEditorViewport>(InViewport)),
Viewport(InViewport.ToWeakPtr()),
PreviewScene(&InPreviewScene.Get()),
Terrian(InTerrian)
{
	PreviewScene->SetLightBrightness(2.0);
	PreviewScene->SetFloorVisibility(false);
	// Hide grid, we don't need this.
	DrawHelper.bDrawGrid = false;
	DrawHelper.bDrawPivot = false;
	DrawHelper.AxesLineThickness = 5;
	DrawHelper.PivotSize = 5;

	//Initiate view
	
	SetViewLocation(FVector(75, 75, 75));
	SetViewRotation(FVector(-75, -75, -75).Rotation());
	
	EngineShowFlags.SetScreenPercentage(true);

	// Set the Default type to Ortho and the XZ Plane
	ELevelViewportType NewViewportType = LVT_Perspective;
	SetViewportType(NewViewportType);

	// View Modes in Persp and Ortho
	SetViewModes(VMI_Lit, VMI_Lit);

	

	SetRealtime(true);

}



void FPangolinEditorViewportClient::OnAssetChanged()
{
	if (!PreviewTerrain)
	{
		PreviewTerrain = Cast<ATerrainActor>(PreviewScene->GetWorld()->SpawnActor(ATerrainActor::StaticClass()));
		
	}
	if (Terrian->ShouldRenderTerrain())
	{
		PreviewTerrain->ApplyTerrianInfo(Terrian->GetHeightMap(), Terrian->GetTerrainSize());
	}
}

void FPangolinEditorViewportClient::Tick(float DeltaSeconds)
{
	FEditorViewportClient::Tick(DeltaSeconds);
	PreviewScene->GetWorld()->Tick(LEVELTICK_All, DeltaSeconds);
}

