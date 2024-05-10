// Fill out your copyright notice in the Description page of Project Settings.


#include "HeightMapWidget.h"
void SHeightMapWidget::Construct(const FArguments& InArgs)
{

	SetSlateBrush(InArgs._Texture.Get());
	OnTextureChanged = InArgs._OnTextureChanged.Get();
	ChildSlot
	[
		SNew(SImage).
		Image_Lambda([this](){return &GetSlateBrush();})
		.DesiredSizeOverride(FVector2D(400.0))
		
	];
}

void SHeightMapWidget::SetSlateBrush(UTexture2D* Texture)
{
	FSlateBrush Tmp;
	Tmp.SetResourceObject(Texture);
	SlateBrush.Set(Tmp);
}
