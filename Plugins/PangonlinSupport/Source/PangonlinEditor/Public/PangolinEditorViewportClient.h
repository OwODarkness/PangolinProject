
#pragma once

#include "CoreMinimal.h"
#include "EditorViewportClient.h"
#include "AdvancedPreviewScene.h"
#include "TerrainActor.h"

/**
 * 
 */
//
class FPangolinEditorViewportClient : public FEditorViewportClient
{
public:
	FPangolinEditorViewportClient(class UPangolinTerrain* InTerrian, const TSharedRef<SEditorViewport> &InViewport, const TSharedRef<FAdvancedPreviewScene>& InPreviewScene);
	
	//void SetPreviewTerrain(ATerrainActor* InTerrainActor);
	void OnAssetChanged();

	virtual void Tick(float DeltaSeconds) override;

	ATerrainActor* GetTerrianActor() const{return PreviewTerrain;}

private:
	TWeakPtr<class SEditorViewport> Viewport;

	FAdvancedPreviewScene* PreviewScene;

	TObjectPtr<ATerrainActor> PreviewTerrain;
	
	class UPangolinTerrain* Terrian;


};
