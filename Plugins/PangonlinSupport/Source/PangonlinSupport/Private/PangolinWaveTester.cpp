// Fill out your copyright notice in the Description page of Project Settings.


#include "PangolinWaveTester.h"
#include "CoastErosionRunnable.h"

// Sets default values
APangolinWaveTester::APangolinWaveTester()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APangolinWaveTester::BeginPlay()
{
	Super::BeginPlay();

	double Degree = 45.;

	FVector OriginPoint = GetActorLocation();
	FVector Conrner1 = OriginPoint;
	Conrner1.X -= 400;
	Conrner1.Y -= 400;

	FVector Conrner2 = OriginPoint;
	Conrner2.X += 400;
	Conrner2.Y -= 400;

	FVector Conrner3 = OriginPoint;
	Conrner3.X -= 400;
	Conrner3.Y += 400;

	FVector Conrner4 = OriginPoint;
	Conrner4.X += 400;
	Conrner4.Y += 400;

	DrawDebugLine(GetWorld(), Conrner1, Conrner3, FColor::White, true);
	DrawDebugLine(GetWorld(), Conrner3, Conrner4, FColor::White, true);

	DrawDebugLine(GetWorld(), Conrner2, Conrner4, FColor::White, true);

	DrawDebugLine(GetWorld(), Conrner1, Conrner2, FColor::White, true);

	for (int32 i = 0; i < 3; i++)
	{
		FVector	StartPoint = (Conrner1 + Conrner2)/2;
		int RandOffset = 400 * FMath::RandRange(-1.,1.);
		StartPoint.X += RandOffset;
		double RandomDegree = FMath::FRandRange(-45., 45);
		
		FVector2D Dir(0, 1);

		FTerrainWave Wave(StartPoint, Dir.GetRotated(RandomDegree), 90., 90., 2.);

		FVector2D VecBorder1 = Wave.Direction.GetRotated(Wave.Azimuth * 0.5);
		FVector2D VecBorder2 = Wave.Direction.GetRotated(-Wave.Azimuth * 0.5);
		DrawDebugLine(GetWorld(), Wave.Location, Wave.Location + 100 * Wave.Radius * (FVector(Wave.Direction, 0.)), FColor::Blue, true);

		DrawDebugLine(GetWorld(), Wave.Location, Wave.Location + 100 * Wave.Radius * (FVector(VecBorder1, 0)), FColor::Red, true);
		DrawDebugLine(GetWorld(), Wave.Location, Wave.Location + 100 * Wave.Radius * (FVector(VecBorder2, 0)), FColor::Red, true);
	}




	//for (int32 i = 0; i < Points.Num(); i++)
	//{
	//	FVector TestPoint = Points[i];

	//	if (Wave.CanWaveCover(TestPoint))
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("Wave (%f, %f) Cover Point(%f, %f)"), Wave.Location.X, Wave.Location.Y, TestPoint.X, TestPoint.Y);
	//	}
	//	else
	//	{
	//		UE_LOG(LogTemp, Error, TEXT("Wave (%f, %f) can not Cover Point(%f, %f)"), Wave.Location.X, Wave.Location.Y, TestPoint.X, TestPoint.Y);
	//		DrawDebugSphere(GetWorld(), GetActorLocation() + TestPoint * 100, 10, 5, FColor::Yellow, true);

	//}

	//}
}

// Called every frame
void APangolinWaveTester::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

