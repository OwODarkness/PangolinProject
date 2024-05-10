// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "TerrainActor.generated.h"


UCLASS()
class PANGONLINSUPPORT_API ATerrainActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATerrainActor();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable)
	void ApplyTerrianInfo(const TArray<double>& InData, int32 Size);
	void RenderTerrianMesh();

	void CreateVerticles();
	void GreateTriangles();
	void GreateTangents();
	//class UProceduralMeshComponent* GetProceduralMeshComponent() const{return ProceduralMeshComponent;}
private:
	UPROPERTY()
	class UProceduralMeshComponent* ProceduralMeshComponent;
	UPROPERTY(EditAnywhere)
	int32 LODLevel = 1;
	UPROPERTY(EditAnywhere) 
	double Scale = 2.;
	UPROPERTY(EditAnywhere)
	double UVScale = 1.;
	
	UPROPERTY(EditAnywhere, Category = "HeightMap Settings")
	FRuntimeFloatCurve HeightRemap;
	UPROPERTY(EditAnywhere)
	double ScaleH = 30.;
	UPROPERTY(EditAnywhere)
	class UMaterialInterface* TerrainMaterial;
	UPROPERTY()
	double SizeX = 4.;
	UPROPERTY()
	double SizeY = 4.;
	UPROPERTY()
	TArray<FVector> VertexBuffer;
	UPROPERTY()
	TArray<int32> TriangleBuffer;
	UPROPERTY()
	TArray<FVector2D> UV0;
	UPROPERTY()
	TArray<double> Data;
	UPROPERTY()
	TArray<struct FProcMeshTangent> Tangents;
	UPROPERTY()
	TArray<FVector> Normals;



};
