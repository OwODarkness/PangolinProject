// Fill out your copyright notice in the Description page of Project Settings.


#include "CoastErosionRunnable.h"
#include "PangolinTerrain.h"

FCoastErosionRunnable::FCoastErosionRunnable(class UPangolinTerrain* InTerrain, const FString& InName, EMapDirectionType InDirectionType, int32 InErosionNum, ECoastErosionSimulateType InCoastErosionSimulateType, int32 InTimeAmount) :
	ThreadName(InName),
	Terrain(InTerrain),
	DirectionType(InDirectionType),
	ErosionNum(InErosionNum),
	CoastErosionSimulateType(InCoastErosionSimulateType),
	TimeAmount(InTimeAmount)
{
}

bool FCoastErosionRunnable::Init()
{
	return true;
}

uint32 FCoastErosionRunnable::Run()
{

	StartTimeStamp = FDateTime::Now().GetTimeOfDay().GetTotalSeconds();

	//UE_LOG(LogTemp, Warning, TEXT("%s is working with coast erosion"), *ThreadName);
	switch (CoastErosionSimulateType)
	{
	case ECoastErosionSimulateType::CES_Normal:
		SimpleErosion();
		break;
	case ECoastErosionSimulateType::CES_Number:
		NumericalModelSimulate();

		break;
	default:
		break;
	}
	return 0;
}

void FCoastErosionRunnable::Stop()
{
}

void FCoastErosionRunnable::Exit()
{
	FinishTimeStamp = FDateTime::Now().GetTimeOfDay().GetTotalSeconds();
	//UE_LOG(LogHeightmap, Display, TEXT("%s is finished, time cost %f"), *ThreadName, FinishTimeStamp - StartTimeStamp);
	CoastErosionAgendaFinished.ExecuteIfBound(FinishTimeStamp);

}

FCoastErosionRunnable::~FCoastErosionRunnable()
{
	UE_LOG(LogHeightmap, Display, TEXT("%s is killed"), *ThreadName);

}

FIntPoint FCoastErosionRunnable::InitStartPoint()
{
	FIntPoint Point;
	int32 TerrainSize = Terrain->GetTerrainSize();
	switch (DirectionType)
	{
	case EMapDirectionType::MDT_North:
		Point.X = TerrainSize / 2;
		Point.Y = 0;
		break;
	case EMapDirectionType::MDT_South:
		Point.X = TerrainSize / 2;
		Point.Y = TerrainSize - 1;
		break;
	case EMapDirectionType::MDT_East:
		Point.X = TerrainSize - 1;
		Point.Y = TerrainSize / 2;
		break;
	case EMapDirectionType::MDT_West:
		Point.X = 0;
		Point.Y = TerrainSize / 2;
		break;
	default:
		break;
	}
	return Point;
}

FVector2D FCoastErosionRunnable::InitStartDirection()
{
	FVector2D Direction;
	switch (DirectionType)
	{
	case EMapDirectionType::MDT_North:
		Direction = FVector2D(0., 1.);
		break;
	case EMapDirectionType::MDT_South:
		Direction = FVector2D(0., -1.);
		break;
	case EMapDirectionType::MDT_East:
		Direction = FVector2D(-1., 0.);
		break;
	case EMapDirectionType::MDT_West:
		Direction = FVector2D(1., 0.);
		break;
	default:
		break;
	}
	return Direction;
}

void FCoastErosionRunnable::InitSeaCell()
{
	double OriginalSeaLevel = BaseSeaLevel;
	int32 TerrainSize = Terrain->GetTerrainSize();
	int32 HalfSize = 0.5 * TerrainSize;
	FIntPoint StartPoint = InitStartPoint();
	FVector2D StartDirection = InitStartDirection();

	//init SeaCellGroup
	double Radius = WaveBaseRadius;
	switch (DirectionType)
	{
	case EMapDirectionType::MDT_North:
	case EMapDirectionType::MDT_South:
		for (int32 x = -HalfSize; x < HalfSize; x++)
		{
			FVector RandLocation = FVector(StartPoint, OriginalSeaLevel);
			RandLocation.X += x;
			RandLocation.Y -= DirectionType == EMapDirectionType::MDT_North ? 5 : -5;
			double RandomDegree = FMath::FRandRange(-45., 45);
			FTerrainWave Wave(RandLocation, StartDirection.GetRotated(RandomDegree), 90., 90., Radius + FMath::RandRange(-10, 10));
			SeaCellGroup.Add(Wave);
		}
		break;
	case EMapDirectionType::MDT_East:
	case EMapDirectionType::MDT_West:
		for (int32 y = -HalfSize; y < HalfSize; y++)
		{
			FVector RandLocation = FVector(StartPoint, OriginalSeaLevel);
			RandLocation.Y += y;
			RandLocation.X -= DirectionType == EMapDirectionType::MDT_East ? -5 : 5;
			double RandomDegree = FMath::FRandRange(-45., 45);
			FTerrainWave Wave = FTerrainWave(RandLocation, StartDirection.GetRotated(RandomDegree), 90., 45., Radius + FMath::RandRange(-10, 10));

			SeaCellGroup.Add(Wave);
		}
		break;
	default:
		break;
	}
}

void FCoastErosionRunnable::ApplyWaveErosion(int32 CurrentTime)
{
	int32 Number = BaseWaveNumber;
	int32 TerrainSize = Terrain->GetTerrainSize();
	int32 WaveAmount = SeaCellGroup.Num();
	for (int32 i = 0; i < Number; i++)
	{
		//选取任意海平面生成海浪
		int32 WaveIndex = FMath::Rand() % WaveAmount;
		FTerrainWave Wave = SeaCellGroup[WaveIndex];

		double DeltaSeaLevel = (0.5 * FMath::Sin(CurrentTime * 2. * PI / (TimeAmount / TimeStep)));
		Wave.Location.Z = FMath::Clamp(Wave.Location.Z + DeltaSeaLevel, 0., 1.);

		FVector2D Dir1 = Wave.Direction;
		FVector2D Dir2 = Dir1.GetRotated(Wave.Azimuth / 2);
		FVector2D Dir3 = Dir1.GetRotated(-Wave.Azimuth / 2);
		FVector2D Point1 = FVector2D(Wave.Location.X, Wave.Location.Y) + Wave.Radius * Dir1;
		FVector2D Point2 = FVector2D(Wave.Location.X, Wave.Location.Y) + Wave.Radius * Dir2;
		FVector2D Point3 = FVector2D(Wave.Location.X, Wave.Location.Y) + Wave.Radius * Dir3;

		int X1, X2, Y1, Y2;
		X1 = FMath::Clamp(FMath::Min3(Point1.X, Point2.X, Point3.X), 0, TerrainSize - 1);
		X2 = FMath::Clamp(FMath::Max3(Point1.X, Point2.X, Point3.X), 0, TerrainSize - 1);
		Y1 = FMath::Clamp(FMath::Min3(Point1.Y, Point2.Y, Point3.Y), 0, TerrainSize - 1);
		Y2 = FMath::Clamp(FMath::Max3(Point1.Y, Point2.Y, Point3.Y), 0, TerrainSize - 1);

		//检测海浪覆盖的地面
		for (int x = X1; x <= X2; x++)
		{
			for (int y = Y1; y <= Y2; y++)
			{
				if (Terrain->IsValidPos(x, y) && !Terrain->IsSeaCell(x, y))
				{
					int32 TargetIndex = Terrain->FindIndex(x, y);

					//若该地块为地面
					double LandHeight = Terrain->GetHeightMapElement(TargetIndex);
					if (Wave.CanWaveCover(FVector(x, y, LandHeight)))
					{
						//判断当前地块是否包含在LandCellMap地块群中
						bool bRecord = LandCellMap.Contains(TargetIndex);

						double S_0 = 1.;

						double Sc = 0.;
						for (int32 CoorX = -1; CoorX <= 1; CoorX++)
						{
							for (int32 CoorY = -1; CoorY <= 1; CoorY++)
							{
								if (CoorX != 0 && CoorY != 0)
								{
									int32 ScaleX = x + CoorX;
									int32 ScaleY = y + CoorY;
									if (Terrain->IsSeaCell(ScaleX, ScaleY))
									{
										if (CoorX * CoorY == 0)
										{
											Sc += 1.;
										}
										else
										{
											Sc += FMath::Sqrt(0.5);
										}
									}

								}
							}
						}

						double DistanceXY = FVector::Distance(FVector(Wave.Location.X, Wave.Location.Y, 0.), FVector(x, y, 0.));
						double DeltaHeight = DistanceXY / Wave.Radius - LandHeight;

						double Distance = FVector::Distance(Wave.Location, FVector(x, y, LandHeight));
						double Varphi = FMath::Acos(DistanceXY / (Distance + 0.0001));

						FVector TargetDir = FVector(x, y, LandHeight) - Wave.Location;
						FVector2D InnerDir1 = Wave.Direction.GetSafeNormal();
						FVector2D InnerDir2 = FVector2D(TargetDir.X, TargetDir.Y).GetSafeNormal();
						double Theta = FMath::Acos(FVector2D::DotProduct(InnerDir1, InnerDir2));

						double DeltaStrength = -Kwave * (Sc + 1) * SeaRho * Gravertiy * DeltaHeight * DeltaHeight * FMath::Abs(FMath::Cos(Varphi - Theta)) * S_0 * TimeStep / 16;

						if (bRecord)
						{
							LandCellMap[TargetIndex].AddDeltaStrength(DeltaStrength);
						}
						else
						{
							FTerrainLandCellInfo LandCell(x, y, 1);
							LandCell.AddDeltaStrength(DeltaStrength);
							LandCellMap.Add(TPair<int32, FTerrainLandCellInfo>(TargetIndex, LandCell));
						}
					}
				}


			}
		}
	}


}



void FCoastErosionRunnable::ApplyUniformErosion()
{
	TMap<int32, FTerrainLandCellInfo> Temp = LandCellMap;
	for (auto& Item : Temp)
	{
		double S_0 = Item.Value.Strength;
		int32 PosX = Item.Value.Location.X;
		int32 PosY = Item.Value.Location.Y;
		double Height = Terrain->GetHeightMapElement(Terrain->FindIndex(PosX, PosY));
		double Sc = 0.;
		for (int32 CoorX = -1; CoorX <= 1; CoorX++)
		{
			for (int32 CoorY = -1; CoorY <= 1; CoorY++)
			{
				if (CoorX != 0 && CoorY != 0)
				{
					int32 ScaleX = PosX + CoorX;
					int32 ScaleY = PosY + CoorY;
					if (Terrain->IsSeaCell(ScaleX, ScaleY))
					{
						if (CoorX * CoorY == 0)
						{
							Sc += 1.;
						}
						else
						{
							Sc += FMath::Sqrt(0.5);
						}
					}

				}
			}
		}
		double DeltaStrength = -Kuniform * (Sc + 1.) * S_0 * TimeStep;
		//Item.Value.AddDeltaStrength(DeltaStrength);

		for (int32 CoorX = -1; CoorX <= 1; CoorX++)
		{
			for (int32 CoorY = -1; CoorY <= 1; CoorY++)
			{
				if (CoorX != 0 && CoorY != 0)
				{
					int32 ScaleX = PosX + CoorX;
					int32 ScaleY = PosY + CoorY;
					double Distance = 0.;
					if (Terrain->IsValidPos(ScaleX, ScaleY) && !Terrain->IsSeaCell(ScaleX, ScaleY))
					{
						if (CoorX * CoorY == 0)
						{
							Distance = FMath::Sqrt(0.5);
						}
						else
						{
							Distance = 1.;
						}
						int32 CornerIndex = Terrain->FindIndex(ScaleX, ScaleY);
						double Elevation = Terrain->GetHeightMapElement(CornerIndex);
						double Rate = Elevation / Distance;
						double WeightDeltaStrength = Rate * DeltaStrength / 10;
						bool bRecord = LandCellMap.Contains(CornerIndex);
						if (bRecord)
						{
							LandCellMap[CornerIndex].AddDeltaStrength(WeightDeltaStrength);
						}
						else
						{
							FTerrainLandCellInfo LandCell(ScaleX, ScaleY, 1);
							LandCell.AddDeltaStrength(WeightDeltaStrength);
							LandCellMap.Add(TPair<int32, FTerrainLandCellInfo>(CornerIndex, LandCell));
						}

					}


				}
			}
		}
	}
}

void FCoastErosionRunnable::SimpleErosion()
{


	int32 TerrainSize = Terrain->GetTerrainSize();
	FIntPoint StartPoint = InitStartPoint();
	TArray<int32> Points;
	Points.Reserve(1000);
	int32 Size = TerrainSize;

	for (int32 i = 0; i < 100; i++)
	{
		FIntPoint Point = StartPoint;
		if (DirectionType == EMapDirectionType::MDT_North || DirectionType == EMapDirectionType::MDT_South)
		{
			Point.X += (double)TerrainSize / 2. * FMath::RandRange(-1., 1.);
		}
		else
		{
			Point.Y += (double)TerrainSize / 2. * FMath::RandRange(-1., 1.);
		}


		int32 Index = Terrain->FindIndex(Point.X, Point.Y);

		Points.Add(Index);

	}

	while (ErosionNum)
	{
		if (Points.Num() > 0)
		{
			int32 Preview = 0;
			int32 CurrentIndex = Points[Preview];
			
			int32 CurrentHeight = Terrain->GetHeightMapElement(CurrentIndex);
			Terrain->SetHeightMapElement(Points[Preview], FMath::Clamp(CurrentHeight-0.3, 0., 1.));
			FIntPoint Loc = Terrain->FindPosByIndex(CurrentIndex);

			if (DirectionType == EMapDirectionType::MDT_North || DirectionType == EMapDirectionType::MDT_South)
			{
				Loc.X = FMath::RandBool() ? Loc.X + 1 : Loc.X - 1;
				Loc.Y = FMath::RandBool() ? Loc.Y : ((DirectionType == EMapDirectionType::MDT_North) ? Loc.Y + 1 : Loc.Y - 1);
			}
			else
			{
				Loc.Y = FMath::RandBool() ? Loc.Y + 1 : Loc.Y - 1;
				Loc.X = FMath::RandBool() ? Loc.X : ((DirectionType == EMapDirectionType::MDT_West) ? Loc.X + 1 : Loc.X - 1);
			}
			int32 Index = Terrain->FindIndex(Loc.X, Loc.Y);
			Points.Add(Index);
			Points.RemoveAt(Preview);
			ErosionNum--;

		}
		else
		{
			break;
		}
	}
}

void FCoastErosionRunnable::NumericalModelSimulate()
{
	SeaCellGroup.Empty();
	LandCellMap.Empty();


	int32 Number = 10;
	InitSeaCell();

	for (int32 time = 0; time < TimeAmount; time += TimeStep)
	{
		ApplyWaveErosion(time);
		ApplyUniformErosion();
		TMap<int32, FTerrainLandCellInfo> Temp = LandCellMap;
		for (const auto& Item : Temp)
		{
			int32 Index = Item.Key;
			FIntPoint Pos = Terrain->FindPosByIndex(Index);
			if (!Terrain->IsSeaCell(Index))
			{
				double CurrentHeight = Terrain->GetHeightMapElement(Index);
				double UpdateHeight = CurrentHeight - 0.2 * (1. - Item.Value.Strength);

				Terrain->SetHeightMapElement(Index, UpdateHeight);
			}
			else
			{
				LandCellMap.Remove(Index);
				FVector WaveLocation = FVector(Pos.X, Pos.Y, BaseSeaLevel);
				double RandomRadian = FMath::DegreesToRadians(FMath::RandRange(0., 360.));
				FVector2D StartDirection(FMath::Cos(RandomRadian), FMath::Sin(RandomRadian));
				FTerrainWave Wave(WaveLocation, StartDirection, 90., 90., WaveBaseRadius + FMath::RandRange(-10, 10));
			}

		}
	}



}

