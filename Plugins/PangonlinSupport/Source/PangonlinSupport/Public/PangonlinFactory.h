// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "PangonlinFactory.generated.h"

/**
 * 
 */
UCLASS()
class PANGONLINSUPPORT_API UPangonlinFactory : public UFactory
{
	GENERATED_BODY()
public:
	UPangonlinFactory();

	UObject* FactoryCreateNew(UClass* Class, UObject* Inparent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn);

};
