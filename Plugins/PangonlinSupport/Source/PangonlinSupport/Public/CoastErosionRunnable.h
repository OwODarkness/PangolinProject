// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

DECLARE_DELEGATE_OneParam(FCoastErosionAgendaFinished, double FinishTimeStamp)
UENUM(BlueprintType)
enum class EMapDirectionType : uint8
{
	MDT_North	UMETA(DisplayName = "North Coast"),
	MDT_South	UMETA(DisplayName = "South Coast"),
	MDT_East	UMETA(DisplayName = "East Coast"),
	MDT_West	UMETA(DisplayName = "West Coast")
};

UENUM(BlueprintType)
enum class ECoastErosionSimulateType : uint8
{
	CES_Normal	UMETA(DisplayName = "Normal Simulate"),
	CES_Number	UMETA(DisplayName = "NumericalModel Simulate"),
};

struct FTerrainWave {

	FTerrainWave():FTerrainWave(FVector(0, 0, 0.), FVector2D(0.0, 1.0), 180., 90., 2.) { }

	FTerrainWave(const FVector& InLocation, const FVector2D& InDirection, double InAzimuth, double InIncident, double InRadius)
	:Location(InLocation),Direction(InDirection), Azimuth(InAzimuth), Incident(InIncident), Radius(InRadius) {}

	FVector Location ;//发射点

	FVector2D Direction;//方向

	double Azimuth;//方位角

	double Incident;//入射角

	double Radius;//半径

	bool CanWaveCover(FVector Point)
	{
		
		double Distance = FVector2D::Distance(FVector2D(Location.X, Location.Y), FVector2D(Point.X, Point.Y));
		if (Distance > Radius)
		{
			//UE_LOG(LogTemp, Error, TEXT("Detect Distance Error %f %f"), Distance, Radius);
			return false;
		}
		
		double WaveHeight = Distance/Radius;
		if (WaveHeight < Point.Z)
		{
			//UE_LOG(LogTemp, Error, TEXT("Detect height Error"));

			return false;
		}

		FVector2D VecBorder1 = Direction.GetRotated(Azimuth * 0.5);
		FVector2D VecBorder2 = Direction.GetRotated(-Azimuth * 0.5);
		FVector2D TargetVector = FVector2D(Point.X - Location.X, Point.Y - Location.Y);
		bool bIsInside = 
		((TargetVector.X * VecBorder1.Y - TargetVector.Y * VecBorder1.X) *
			(TargetVector.X * VecBorder2.Y - TargetVector.Y * VecBorder2.X)) < 0. ? true : false; 
		if (!bIsInside)
		{
			//UE_LOG(LogTemp, Error, TEXT("Detect inside Error"));
			return false;
		}

		return true;
	}
};

struct FTerrainLandCellInfo {

	FTerrainLandCellInfo():FTerrainLandCellInfo(0,0,1.) {}
	FTerrainLandCellInfo(int32 X, int32 Y, double InStrength):
	Location(FIntPoint(X, Y)), Strength(InStrength){}

	FIntPoint Location;
	double Strength;

	void SetStrength(double Value)
	{
		Strength = FMath::Clamp(Value, 0., 1.);
	}
	void AddDeltaStrength(double DeltaValue)
	{
		SetStrength(Strength + DeltaValue);
	}
};

class  FCoastErosionRunnable : public FRunnable
{
public:
	FCoastErosionRunnable(class UPangolinTerrain* InTerrain, const FString& InName, EMapDirectionType InDirectionType, int32 InErosionNum, ECoastErosionSimulateType CoastErosionSimulateType, int32 TimeAmount);

	bool Init() override;

	uint32 Run() override;

	void Stop() override;

	void Exit() override;

	FIntPoint InitStartPoint();

	FVector2D InitStartDirection();

	void InitSeaCell();


	void ApplyWaveErosion(int32 CurrentTime);
	
	void ApplyUniformErosion();

	~FCoastErosionRunnable();
public:
	FString ThreadName;

	FCoastErosionAgendaFinished CoastErosionAgendaFinished;
private:

	class UPangolinTerrain* Terrain;

	EMapDirectionType DirectionType;

	int32 ErosionNum = 2000;

	ECoastErosionSimulateType CoastErosionSimulateType;

	void SimpleErosion();

	void NumericalModelSimulate();


	double Kwave = 1.0;//海浪易蚀系数
	
	double Kuniform = 1.0;//均匀易蚀系数

	double Gravertiy = 9.8;//重力

	double SeaRho = 1.04;//海水密度

	double BaseSeaLevel = 0.5;

	double WaveBaseRadius = 20.;

	int32 BaseWaveNumber = 10.;//海浪数量

	int32 TimeAmount = 10;

	int32 TimeStep = 1;

	TArray<FTerrainWave> SeaCellGroup;
	
	TMap<int32, FTerrainLandCellInfo> LandCellMap;

	double StartTimeStamp = 0.;
	double FinishTimeStamp = 0.;

};
