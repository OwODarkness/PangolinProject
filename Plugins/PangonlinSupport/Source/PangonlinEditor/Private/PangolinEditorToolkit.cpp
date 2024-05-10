// Fill out your copyright notice in the Description page of Project Settings.


#include "PangolinEditorToolkit.h"
#include "PangolinTerrain.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Exporters/Exporter.h"

void FPangolinEditorToolkit::InitEditor(const TArray<UObject*> InObjects)
{
	Terrian = Cast<UPangolinTerrain>(InObjects[0]);
	check(Terrian);
	Terrian->GenerateHeightMap();
	
	const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("PangonlinTerrianEditorLayout_v1.0")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Horizontal)
			->Split
			(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Vertical)
				->SetSizeCoefficient(0.2f)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.5f)
					->AddTab("PangonlinTerrianGrayScaleHeightMapTab", ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.5f)
					->AddTab("PangonlinTerrianColorScaleHeightMapTab", ETabState::OpenedTab)
				)
)
			->Split
			(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Vertical)
				->SetSizeCoefficient(0.6)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.7f)
					->AddTab("PangonlinTerrianViewportTab", ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.3f)
					->AddTab("PangonlinTerrianNoisePlotTab", ETabState::OpenedTab)
				)
			
			)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.2f)
				->AddTab("PangonlinTerrianDetailsTab", ETabState::OpenedTab)
			)
		);
	FAssetEditorToolkit::InitAssetEditor(EToolkitMode::Standalone, {}, "PangonlinTerrianEditor", Layout, true, true, InObjects);

	

}

void FPangolinEditorToolkit::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
	//Export Button
	auto Button = SNew(SButton)
		.Text(FText::FromString("Export"))
		.ButtonColorAndOpacity(FColor::Cyan)
		.OnClicked(FOnClicked::CreateSP(this, &FPangolinEditorToolkit::OnExportButtonClicked));
	AddToolbarWidget(Button);
	
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(INVTEXT("PangonlinTerrianEditor"));
	// GrayScale Height Map Tab
	GrayScaleNoiseWidget = SNew(SHeightMapWidget)
		.Texture(Terrian->GetGrayScaleTexture())
		.OnTextureChanged(FOnTextureChanged::CreateSP(this, &FPangolinEditorToolkit::SetGrayScaleNoiseTexture));
	InTabManager->RegisterTabSpawner(FName("PangonlinTerrianGrayScaleHeightMapTab"), FOnSpawnTab::CreateLambda([=](const FSpawnTabArgs& SpawnTabArgs) {
		
		return SNew(SDockTab).ContentPadding(FMargin(3))
		[
			SNew(SScaleBox)					
			[
				SNew(SBorder)
				[
					GrayScaleNoiseWidget.ToSharedRef()
				]
			]
		];
	})).
	SetDisplayName(FText::FromString("GrayScale HeightMap")).
	SetGroup(WorkspaceMenuCategory.ToSharedRef());

	// ColorScale Height Map Tab
	ColorScaleNoiseWidget = SNew(SHeightMapWidget)
		.Texture(Terrian->GetColorScaleTexture())
		.OnTextureChanged(FOnTextureChanged::CreateSP(this, &FPangolinEditorToolkit::SetColorScaleNoiseTexture));

	InTabManager->RegisterTabSpawner(FName("PangonlinTerrianColorScaleHeightMapTab"), FOnSpawnTab::CreateLambda([=](const FSpawnTabArgs& SpawnTabArgs) {
		return SNew(SDockTab).ContentPadding(FMargin(3.f))
			[
				SNew(SScaleBox)
				[
					SNew(SBorder)
					[
						ColorScaleNoiseWidget.ToSharedRef()
					]
				]
			];
		})).
		SetDisplayName(FText::FromString("ColorScale HeightMap")).
			SetGroup(WorkspaceMenuCategory.ToSharedRef());

		TerrianViewport = SNew(SPangolinEditorViewport).Terrian(Terrian);

	InTabManager->RegisterTabSpawner(FName("PangonlinTerrianViewportTab"), FOnSpawnTab::CreateLambda([=](const FSpawnTabArgs& SpawnTabArgs) {
		return SNew(SDockTab)
			[
				TerrianViewport.ToSharedRef()
			];
		})).SetDisplayName(INVTEXT("Viewport"))
			.SetGroup(WorkspaceMenuCategory.ToSharedRef());


		NoisePlotWidget = SNew(STerrianNoisePlotWidget).
			Points(Terrian->GetNoise1D(3, true))
			.OnNoisePlotUpdated(FOnNoisePlotUpdated::CreateSP(this, &FPangolinEditorToolkit::OnNoisePlotUpdateEvent));

		InTabManager->RegisterTabSpawner(FName("PangonlinTerrianNoisePlotTab"), FOnSpawnTab::CreateLambda([=](const FSpawnTabArgs& SpawnTabArgs) {
			return SNew(SDockTab)
				[
					SNew(SBorder)
					[
						NoisePlotWidget.ToSharedRef()
					]
				];
			})).SetDisplayName(INVTEXT("NoisePlot"))
				.SetGroup(WorkspaceMenuCategory.ToSharedRef());


		//Details
		FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		FDetailsViewArgs DetailsViewArgs;
		DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
		TSharedRef<IDetailsView> DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
		DetailsView->SetObjects(TArray<UObject*>{Terrian});
		InTabManager->RegisterTabSpawner(FName("PangonlinTerrianDetailsTab"), FOnSpawnTab::CreateLambda([=](const FSpawnTabArgs& SpawnTabArgs) {
			return SNew(SDockTab)
				[
					SNew(SVerticalBox)
					+SVerticalBox::Slot().FillHeight(0.1)
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot()
						.HAlign(EHorizontalAlignment::HAlign_Center)
						.VAlign(EVerticalAlignment::VAlign_Center)
						[
							SNew(SButton).
							Text(FText::FromString("Generate")).
							HAlign(EHorizontalAlignment::HAlign_Center)
							.OnClicked(FOnClicked::CreateSP(this, &FPangolinEditorToolkit::OnTerrianGenerateButtonClicked))
						]
			+ SHorizontalBox::Slot()
				.HAlign(EHorizontalAlignment::HAlign_Center)
				.VAlign(EVerticalAlignment::VAlign_Center)
				[	
					SNew(SCheckBox).IsChecked(ECheckBoxState::Checked).OnCheckStateChanged_Lambda([this](ECheckBoxState State){
							NoisePlotWidget->SetUIVisibility( State == ECheckBoxState::Checked ? EVisibility::Visible : EVisibility::Collapsed);
						})
				]

					]
					+SVerticalBox::Slot()
					[
						DetailsView
					]
				];
			}))
			.SetDisplayName(INVTEXT("Details"))
				.SetGroup(WorkspaceMenuCategory.ToSharedRef());

			TSharedRef<IDetailsView> TerrianDetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
			TerrianDetailsView->SetObjects(TArray<UObject*>{TerrianViewport->GetTerrianActor()});
			InTabManager->RegisterTabSpawner(FName("TerrianActorDetailsTab"), FOnSpawnTab::CreateLambda([=](const FSpawnTabArgs& SpawnTabArgs) {
				return SNew(SDockTab)[
					TerrianDetailsView
				];

				})).SetDisplayName(INVTEXT("TerrianObjectDetails"))
					.SetGroup(WorkspaceMenuCategory.ToSharedRef());

}

void FPangolinEditorToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
	InTabManager->UnregisterTabSpawner(FName("PangonlinTerrianGrayScaleHeightMapTab"));
	InTabManager->UnregisterTabSpawner(FName("PangonlinTerrianColorScaleHeightMapTab"));
	InTabManager->UnregisterTabSpawner(FName("PangonlinTerrianViewportTab"));
	InTabManager->UnregisterTabSpawner(FName("PangonlinTerrianDetailsTab"));
	
}

FReply FPangolinEditorToolkit::OnTerrianGenerateButtonClicked()
{
	Terrian->GenerateHeightMap();

	GrayScaleNoiseWidget.ToSharedRef().Get().OnTextureChanged.ExecuteIfBound(Terrian->GetGrayScaleTexture());
	ColorScaleNoiseWidget.ToSharedRef().Get().OnTextureChanged.ExecuteIfBound(Terrian->GetColorScaleTexture());
	NoisePlotWidget.Get()->SetPoints(Terrian->GetNoise1D(NoisePlotWidget.Get()->GetCurrentIndex(), NoisePlotWidget.Get()->GetAxisFlag()));
	TerrianViewport->PostTerrainInfoUpdated();
	return FReply::Handled();
}

void FPangolinEditorToolkit::SetGrayScaleNoiseTexture(UTexture2D* Texture)
{
	GrayScaleNoiseWidget.ToSharedRef().Get().SetSlateBrush(Texture);
}

void FPangolinEditorToolkit::SetColorScaleNoiseTexture(UTexture2D* Texture)
{
	ColorScaleNoiseWidget.ToSharedRef().Get().SetSlateBrush(Texture);
}

FReply FPangolinEditorToolkit::OnExportButtonClicked()
{
	//Should be implement
	return FReply::Handled();
}

void FPangolinEditorToolkit::OnNoisePlotUpdateEvent(int32 Index, bool Flag)
{
	NoisePlotWidget.Get()->SetPoints(Terrian->GetNoise1D(Index, Flag));
}
