// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
DECLARE_DELEGATE_OneParam(FOnTextureChanged, UTexture2D*)

class  SHeightMapWidget : public SCompoundWidget
{

public:
	SLATE_BEGIN_ARGS(SHeightMapWidget):_Texture(nullptr){}

	SLATE_ATTRIBUTE(UTexture2D*, Texture)

	SLATE_ATTRIBUTE(FOnTextureChanged, OnTextureChanged)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	const FSlateBrush& GetSlateBrush() const  {return SlateBrush.Get(); }

	void SetSlateBrush(UTexture2D* Texture);

private:
	TAttribute<FSlateBrush> SlateBrush;
public:
	FOnTextureChanged OnTextureChanged;

};
