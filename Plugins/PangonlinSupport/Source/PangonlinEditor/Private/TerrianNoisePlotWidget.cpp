// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrianNoisePlotWidget.h"

void STerrianNoisePlotWidget::Construct(FArguments InArguments)
{
	Points = InArguments._Points.Get();
	OnNoisePlotUpdated = InArguments._OnNoisePlotUpdated.Get();

	UpdateButton = SNew(SButton).Text(FText::FromString("Update")).
		OnClicked(FOnClicked::CreateSP(this, &STerrianNoisePlotWidget::OnUpdateButtonClicked));
	InputBox = SNew(SSpinBox<uint32>).MinValue(0).MaxValue(511);
	CheckBox1 = SNew(SCheckBox).IsChecked(ECheckBoxState::Checked).OnCheckStateChanged(FOnCheckStateChanged::CreateSP(this, &STerrianNoisePlotWidget::OnCheckBox1Clicked));
	CheckBox2 =  SNew(SCheckBox).IsChecked(ECheckBoxState::Unchecked).OnCheckStateChanged(FOnCheckStateChanged::CreateSP(this, &STerrianNoisePlotWidget::OnCheckBox2Clicked));
	ChildSlot
	[
		SNew(SOverlay)
		+SOverlay::Slot()
		[
			SNew(SBorder)
		]
		+SOverlay::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Left)
			.VAlign(EVerticalAlignment::VAlign_Top)
			.AutoWidth()
			[
				UpdateButton.ToSharedRef()
			]
			+ SHorizontalBox::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Left)
			.VAlign(EVerticalAlignment::VAlign_Top)
			
			[
				SNew(SBorder)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					[
						SNew(STextBlock).Text(FText::FromString("Index: "))
						.ColorAndOpacity(FColor::Cyan)
						.Justification(ETextJustify::Center)
					]
					+ SHorizontalBox::Slot().AutoWidth()
					[
						InputBox.ToSharedRef()
					]
				]
			]
			+SHorizontalBox::Slot()
				.HAlign(EHorizontalAlignment::HAlign_Left)
				.VAlign(EVerticalAlignment::VAlign_Top)
				[
					SNew(SBorder)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
				[
					SNew(STextBlock).Text(FText::FromString("Horizontal: "))
				]
						+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					CheckBox1.ToSharedRef()
				]
						+ SHorizontalBox::Slot()
							[
								SNew(STextBlock).Text(FText::FromString("Vertical: "))
							]
			+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					CheckBox2.ToSharedRef()

				]
					]


			]

		]
	
	];
	
}

int32 STerrianNoisePlotWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	
	const int32 PointsNum = Points.Num();
	TArray<FVector2D> Result;
	Result.Reserve(PointsNum);
	const FTransform2D PointsTransform = GetPoinsTransform(AllottedGeometry);
	for (int32 Index = 0; Index < PointsNum; Index++)
	{
		const double X = (double)Index / ((double)PointsNum - 1.0);
		const double Y = Points[Index];
		Result.Add(PointsTransform.TransformPoint(FVector2D(X, Y)));
	}
	SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), Result);

	TArray<FVector2D> Axis;
	Axis.Add(PointsTransform.TransformPoint(FVector2D(0, 0)));
	Axis.Add(PointsTransform.TransformPoint(FVector2D(1, 0)));
	FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), Axis, ESlateDrawEffect::None, FLinearColor::Yellow);
	Axis.Empty();
	Axis.Add(PointsTransform.TransformPoint(FVector2D(0, 0)));
	Axis.Add(PointsTransform.TransformPoint(FVector2D(0, 1)));
	FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), Axis, ESlateDrawEffect::None, FLinearColor::Yellow);


	int32 ScaleSize = 16;
	int32 XOffset = PointsNum/ (ScaleSize-1);
	for (int32 i = 0; i < ScaleSize; i++)
	{
		const double X = XOffset*i / ((double)PointsNum - 1.0);
		const double Y = 0.;
		Axis.Empty();
		Axis.Add(PointsTransform.TransformPoint(FVector2D(X, Y)));
		Axis.Add(PointsTransform.TransformPoint(FVector2D(X, Y-0.1)));
		FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), Axis, ESlateDrawEffect::None, FLinearColor::Yellow);
	}
	ScaleSize = 5;
	double YOffset = 1.0 / (double)(ScaleSize - 1);
	for (int32 i = 0; i < ScaleSize; i++)
	{
		const double X = 0;
		const double Y = i*YOffset;
		Axis.Empty();
		Axis.Add(PointsTransform.TransformPoint(FVector2D(X, Y)));
		Axis.Add(PointsTransform.TransformPoint(FVector2D(X-1, Y)));
		FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), Axis, ESlateDrawEffect::None, FLinearColor::Yellow);
	}
	return LayerId;
}

void STerrianNoisePlotWidget::SetPoints(const TArray<double>& InPoints)
{
	Points.Empty();
	Points = InPoints;
}

int32 STerrianNoisePlotWidget::GetCurrentIndex() const
{
	return InputBox.Get()->GetValue();
}

bool STerrianNoisePlotWidget::GetAxisFlag() const
{
	return (bool)CurrentChoice;
}

void STerrianNoisePlotWidget::SetUIVisibility(EVisibility InVisibility)
{
	ChildSlot.GetChildAt(0).Get().SetVisibility(InVisibility);
}

FTransform2D STerrianNoisePlotWidget::GetPoinsTransform(const FGeometry& AllottedGeomerty) const
{
	const double Margin = 0.05 * AllottedGeomerty.GetLocalSize().GetMin();
	const FScale2D Scale((AllottedGeomerty.GetLocalSize() - 2.0 * Margin) * FVector2D(1.0, -1.0));
	const FVector2D Translation(Margin, AllottedGeomerty.GetLocalSize().Y - Margin);
	return FTransform2D(Scale, Translation);
}

FReply STerrianNoisePlotWidget::OnUpdateButtonClicked()
{
	OnNoisePlotUpdated.ExecuteIfBound(GetCurrentIndex(), GetAxisFlag());
	return FReply::Handled();
}

void STerrianNoisePlotWidget::OnCheckBox1Clicked(ECheckBoxState State)
{
	if (State == ECheckBoxState::Checked)
	{
		CheckBox2.Get()->SetIsChecked(ECheckBoxState::Unchecked);
		CurrentChoice = ERadioChoice::ERC_BoxH;
	}
	else
	{
		CheckBox1.Get()->SetIsChecked(ECheckBoxState::Checked);
	}
}

void STerrianNoisePlotWidget::OnCheckBox2Clicked(ECheckBoxState State)
{
	if (State == ECheckBoxState::Checked)
	{
		CheckBox1.Get()->SetIsChecked(ECheckBoxState::Unchecked);
		CurrentChoice = ERadioChoice::ERC_BoxV;
	}
	else
	{
		CheckBox2.Get()->SetIsChecked(ECheckBoxState::Checked);
	}
}

