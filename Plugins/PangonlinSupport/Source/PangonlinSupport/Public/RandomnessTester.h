// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RandomnessTester.generated.h"

UCLASS()
class PANGONLINSUPPORT_API ARandomnessTester : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARandomnessTester();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void GenerateHeightMap(int32 Seed, TArray<double>& Storage);

	double GetScaleCoordinateBasedLattice(double Origin, double MapSize, double LacSize, double Offset = 0.0, double Frequency = 1.0) const { return Origin * LacSize * Frequency / MapSize + Offset; }

	double NormalizePerlinNoiseValue(double Value) const{return 0.5 + 0.5 * Value;}

	double ApplyFBM(double X, double Y, double Offset);

	UFUNCTION(BlueprintCallable)
	void TestTask();

	bool CompareHeightmap(int32 SeedA, int32 SeedB);
private:
	UPROPERTY(EditAnywhere)
	TArray<FIntPoint> SeedPairs;
	UPROPERTY(EditAnywhere)
	double HeightMapSize;
	UPROPERTY(EditAnywhere)
	double LatticeSize;

	UPROPERTY(EditAnywhere)
	int32 Octaves = 4;
	UPROPERTY(EditAnywhere)
	double Lacunarity= 2.0;

	UPROPERTY(EditAnywhere)
	double Persistance = 0.5;

	UPROPERTY(EditAnywhere)
	double Errors = 0.05;

	UPROPERTY()
	TArray<bool> Results;

	TArray<double> HeightMapA;
	TArray<double> HeightMapB;

	
};
