// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SSpinBox.h"
/**
 * 
 */

UENUM()
enum class ERadioChoice : uint8
{
	ERC_BoxH,
	ERC_BoxV
};
DECLARE_DELEGATE_TwoParams(FOnNoisePlotUpdated, int32, bool)

class  STerrianNoisePlotWidget: public SCompoundWidget
{
	SLATE_BEGIN_ARGS(STerrianNoisePlotWidget){}

	SLATE_ATTRIBUTE(TArray<double>, Points)
	SLATE_ATTRIBUTE(FOnNoisePlotUpdated, OnNoisePlotUpdated)
	SLATE_END_ARGS()

public:
	void Construct( FArguments InArguments);
	int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	void SetPoints(const TArray<double>& InPoints);

	int32 GetCurrentIndex() const;

	bool GetAxisFlag() const;

	void SetUIVisibility(EVisibility InVisibility);

	FOnNoisePlotUpdated OnNoisePlotUpdated;
protected:
	FTransform2D GetPoinsTransform(const FGeometry& AllottedGeomerty) const;

private:
	ERadioChoice CurrentChoice = ERadioChoice::ERC_BoxH;

	TArray<double> Points;

	TSharedPtr<SButton> UpdateButton; 
	TSharedPtr<SSpinBox<uint32>> InputBox;

	TSharedPtr<SCheckBox> CheckBox1;
	TSharedPtr<SCheckBox> CheckBox2;
	

private:
	UFUNCTION()
	FReply OnUpdateButtonClicked();

	UFUNCTION()
	void OnCheckBox1Clicked(ECheckBoxState State);
	UFUNCTION()
	void OnCheckBox2Clicked(ECheckBoxState State);

};
