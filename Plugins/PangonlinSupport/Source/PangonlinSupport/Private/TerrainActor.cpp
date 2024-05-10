// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainActor.h"
#include "PangolinTerrain.h"
#include "Materials/MaterialInterface.h"
#include "KismetProceduralMeshLibrary.h"
#include "ProceduralMeshComponent.h"


ATerrainActor::ATerrainActor()
{
	ProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComponent"));
	ProceduralMeshComponent->SetupAttachment(GetRootComponent());
	HeightRemap.GetRichCurve()->AddKey(0.0, 0.0);
	HeightRemap.GetRichCurve()->AddKey(1.0, 1.0);
}

void ATerrainActor::BeginPlay()
{
	Super::BeginPlay();
	RenderTerrianMesh();
}

void ATerrainActor::RenderTerrianMesh()
{
	VertexBuffer.Empty();
	TriangleBuffer.Empty();
	UV0.Empty();
	Tangents.Empty();
	Normals.Empty();

	CreateVerticles();
	GreateTriangles();

	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(VertexBuffer, TriangleBuffer, UV0, Normals, Tangents);
	
	ProceduralMeshComponent->CreateMeshSection(0, VertexBuffer, TriangleBuffer, Normals,UV0, TArray<FColor>(), Tangents, true);
	ProceduralMeshComponent->SetMaterial(0, TerrainMaterial);	
}

void ATerrainActor::ApplyTerrianInfo(const TArray<double>& InData, int32 Size)
{
	SizeX = SizeY = FMath::Sqrt((double)InData.Num());
	Data = InData;
	RenderTerrianMesh();
	return ;

}

void ATerrainActor::CreateVerticles()
{
	VertexBuffer.Reserve(SizeX * SizeY);
	UV0.Reserve(SizeX * SizeY);
	int32 Offset = 16;
	for (int32 x = -Offset ; x < SizeX+Offset; x+=LODLevel)
	{
		for (int32 y = -Offset; y < SizeY + Offset; y+=LODLevel)
		{
			if (Data.Num() > 0)
			{
				FVector V;
				if (x >= 0 && x < SizeX && y >= 0 && y < SizeY)
				{
					V = FVector(x * Scale, y * Scale, ScaleH * 2 * (HeightRemap.GetRichCurve()->Eval(Data[x * SizeY + y])));
				}
				else
				{
					V = FVector(x * Scale, y * Scale, ScaleH * 2 * (HeightRemap.GetRichCurve()->Eval(0)));
				}
				VertexBuffer.Add(V);

			}
			UV0.Add(FVector2D(x * UVScale /SizeX ,y * UVScale / SizeY));

		}
	}
	
}

void ATerrainActor::GreateTriangles()
{
	TriangleBuffer.Reserve(6 * (SizeX-1) * (SizeY-1));
	int32 Index = 0;
	int32 N  = FMath::Sqrt((double)VertexBuffer.Num()) - 1;
	for (int32 i = 0; i < N; i++)
	{
		for (int32 j = 0; j < N; j++)
		{
			TriangleBuffer.Add(Index);
			TriangleBuffer.Add(Index + 1);
			TriangleBuffer.Add(Index + N + 2);


			TriangleBuffer.Add(Index + N + 2);
			TriangleBuffer.Add(Index + N + 1);
			TriangleBuffer.Add(Index);

			Index++;
		}
		Index++;
	}
}

void ATerrainActor::GreateTangents()
{

	TArray<struct FProcMeshTangent> OutTangents;
	TArray<FVector> OutNormals;
	TArray<FVector> OutVertices;
	TArray<FVector2D> OutUV0;

	OutVertices.Reserve((SizeX + 2) * (SizeY + 2));
	OutUV0.Reserve((SizeX + 2) * (SizeY + 2));
	OutNormals.Reserve((SizeX + 2) * (SizeY + 2));
	OutTangents.Reserve((SizeX + 2) * (SizeY + 2));

	for (int32 i = -1; i <= SizeX+1; i++)
	{
		for (int32 j = -1; j <= SizeY+1; j++)
		{
			bool IsOriX = i >= 0 && i <= SizeX;
			bool IsOriY = j >= 0 && j <= SizeY;
			if (IsOriX && IsOriY)
			{
				 OutVertices.Add(VertexBuffer[i*SizeY + j]);
				 OutUV0.Add(UV0[i * SizeY + j]);
				 OutNormals.Add(Normals[i * SizeY + j]);
				 OutTangents.Add(Tangents[i * SizeY + j]);
			}
		}
	}

}

