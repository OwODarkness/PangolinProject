// Fill out your copyright notice in the Description page of Project Settings.


#include "PangonlinFactory.h"
#include "PangolinTerrain.h"

UPangonlinFactory::UPangonlinFactory()
{
	bEditAfterNew = true;
	bCreateNew = true;
	SupportedClass = UPangolinTerrain::StaticClass();
}

UObject* UPangonlinFactory::FactoryCreateNew(UClass* Class, UObject* Inparent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UPangolinTerrain>(Inparent, Class, Name, Flags, Context);
}
