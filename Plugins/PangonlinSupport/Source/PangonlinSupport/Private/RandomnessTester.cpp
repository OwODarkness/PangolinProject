// Fill out your copyright notice in the Description page of Project Settings.


#include "RandomnessTester.h"

// Sets default values
ARandomnessTester::ARandomnessTester()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARandomnessTester::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARandomnessTester::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARandomnessTester::GenerateHeightMap(int32 Seed, TArray<double>& HeightMap)
{
	HeightMap.Empty();
	HeightMap.Reserve(HeightMapSize * HeightMapSize);
	int32 MurMurRandom = MurmurFinalize32(Seed) % 100;
	double RandomOffset = LatticeSize * FMath::Sin((double)MurMurRandom * PI / 180.0);
	int32 Offset = 30;
	for (int32 y = 0; y < HeightMapSize; y++)
	{
		for (int32 x = 0; x < HeightMapSize; x++)
		{
			double Height = 0.;
			Height = NormalizePerlinNoiseValue(ApplyFBM(x, y, RandomOffset));
			
			HeightMap.Add(Height);

		}

	}
}

double ARandomnessTester::ApplyFBM(double X, double Y, double Offset)
{
	double ScaledX = 0.;
	double ScaledY = 0.;
	double Frequency = 1.0;
	double Amplitude = 1.0;
	double Result = 0.0;
	for (int32 i = 0; i < Octaves; i++)
	{
		ScaledX = GetScaleCoordinateBasedLattice(X, HeightMapSize, LatticeSize, Offset, Frequency);
		ScaledY = GetScaleCoordinateBasedLattice(Y, HeightMapSize, LatticeSize, Offset, Frequency);
		Result += Amplitude * FMath::PerlinNoise2D(FVector2D(ScaledX, ScaledY));
		Frequency *= Lacunarity;
		Amplitude *= Persistance;
	}
	return Result;
}

void ARandomnessTester::TestTask()
{
	Results.Empty();
	for (const FIntPoint& Item : SeedPairs)
	{
		Results.Add(CompareHeightmap(Item.X, Item.Y));
	}

	for (int32 i = 0; i < SeedPairs.Num(); i++)
	{
		FString S;
		S += FString::FromInt(i+1) ;
		S += " SeedA" + FString::FromInt(SeedPairs[i].X);
		S += " SeedB" + FString::FromInt(SeedPairs[i].Y);
		S += " Result:" + (Results[i]==true) ? "True" : "False";
		UE_LOG(LogTemp, Error, TEXT("%s"), *S);
	}
}

bool ARandomnessTester::CompareHeightmap(int32 SeedA, int32 SeedB)
{
	GenerateHeightMap(SeedA, HeightMapA);
	GenerateHeightMap(SeedB, HeightMapB);
	for (int32 i = 0; i < HeightMapA.Num(); i++)
	{
		if (!FMath::IsNearlyEqual(HeightMapA[i], HeightMapB[i]))
		{
			UE_LOG(LogTemp, Error,TEXT("Different"));
			return false;
		}
	}

	return true;
}

