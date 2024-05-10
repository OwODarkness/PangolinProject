// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CoastErosionRunnable.h"
#include "PangolinTerrain.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHeightmap, Log, All);

namespace HeightMapDefaultSize {
	const int32 Size_128 = 128;
	const int32 Size_256 = 256;
	const int32 Size_512 = 512;
	const int32 Size_1024 = 1024;
	const int32 Size_2048 = 2048;
}
// Enum Layer Type 
UENUM(BlueprintType)
enum class ETerrainLayerType : uint8
{
	None,
	ETT_DeepWater		UMETA(DisplayName = "DeepWater"),
	ETT_Water			UMETA(DisplayName = "Water"),
	ETT_Sand			UMETA(DisplayName = "Sand"),
	ETT_GrassBottom		UMETA(DisplayName = "GrassBottom"),
	ETT_Grass			UMETA(DisplayName = "Grass"),
	ETT_Rock			UMETA(DisplayName = "Rock"),
	ETT_Mountain		UMETA(DisplayName = "Mountain")
};

UENUM(BlueprintType)
enum class EHeightmapType : uint8
{
	HMT_Perlin	UMETA(DisplayName = "Perlin Noise"),
	HMT_DiamondSquare	UMETA(DisplayName = "DiamondSquare")
};

UENUM(BlueprintType)
enum class EConvKernelType : uint8
{
	None,
	EKT_MeanKernel	UMETA(DisplayName = "Mean Kernel"),
	EKT_GaussKernel	UMETA(DisplayName = "Gauss Kernel"),
};

USTRUCT(BlueprintType)
struct FTerrainLayerInfo
{
	GENERATED_USTRUCT_BODY()	

	FTerrainLayerInfo():UpperBound(1.0),LayerColor(FColor::Black) {}

	FTerrainLayerInfo(double Bound, FColor Color):
	UpperBound(Bound), LayerColor(Color) {}

	UPROPERTY(EditAnywhere)
	double UpperBound;	

	UPROPERTY(EditAnywhere)
	FColor LayerColor;

};

USTRUCT(BlueprintType)
struct FCoastErosionInfo
{
	GENERATED_USTRUCT_BODY()

	FCoastErosionInfo():bDisable(false), ErosionUnitNum(2000) {}

	FCoastErosionInfo(bool InbDisable, int32 InErosionUnitNum):bDisable(InbDisable), ErosionUnitNum(InErosionUnitNum) {}

	UPROPERTY(EditAnywhere)
	bool bDisable;

	UPROPERTY(EditAnywhere)
	int32 ErosionUnitNum;
};



class UPangolinHeightMap 
{
public:

	UPangolinHeightMap(int32 Size);

	FORCEINLINE TArray<double> GetHeightMap() const { return Data; }

	void SetHeightMap(const TArray<double> &InData);

	int32 GetTerrainSize() const { return TerrainSize; }

	void SetTerrainSize(int32 Size);

	bool CheckElement(int32 Index);

	double GetElement(int32 Index);

	void SetElement(int32 Index, double Value);
private:

	UPROPERTY()
	int32 TerrainSize;

	UPROPERTY(DuplicateTransient)
	TArray<double> Data;
};

UCLASS(BlueprintType)
class PANGONLINSUPPORT_API UPangolinTerrain : public UObject
{
	GENERATED_BODY()
public:
	UPangolinTerrain();

public:
	UFUNCTION(BlueprintCallable)
	void GenerateHeightMapByPerlin();

	UFUNCTION(BlueprintCallable)
	void RenderNoiseTexture();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UTexture2D* GetGrayScaleTexture() const { return GrayScaleTexture; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UTexture2D* GetColorScaleTexture() const { return ColorScaleTexture; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE TArray<double> GetHeightMap() const {return HeightMap;}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetTerrainSize() const{return TerrainSize;}

	UFUNCTION(BlueprintCallable)
	void SetHeightMapSize(int32 InSize);

	UFUNCTION(BlueprintCallable)
	void SetHeightMapElement(int32 Index, double Value);

	UFUNCTION(BlueprintCallable)
	double GetHeightMapElement(int32 Index, bool bShowLogInfo = false) const;

	UFUNCTION(BlueprintCallable)
	int32 FindIndex(int32 X, int32 Y, bool bShowLogInfo = false) const;

	UFUNCTION(BlueprintCallable)
	FIntPoint FindPosByIndex(int32 Index) const;

	UFUNCTION()
	TArray<double> GetNoise1D(int32 Index, bool IsHorizontal) const;

	UFUNCTION()
	void GenerateHeightMapByDiamondSquare();

	UFUNCTION()
	void CoastErosion();

	UFUNCTION(BlueprintCallable)
	void GenerateHeightMap();

	UFUNCTION(BlueprintCallable)
	bool IsSeaCell(int32 X, int32 Y) const;
	bool IsSeaCell(int32 Index) const;

	UFUNCTION()
	bool IsValidPos(int32 X, int32 Y) const;

	UFUNCTION()
	bool IsValidIndex(int32 Index) const;

	UFUNCTION()
	bool ShouldRenderTerrain() const{return bRenderTerrain;}

	UFUNCTION(BlueprintCallable)
	void SetCoastErosionTime(int32 Time){ CoastErosionTime  = Time;}

	UFUNCTION(BlueprintCallable)
	void SetShouldApplyMulti(bool bUse){bUseMultiThread = bUse;}

private:
	void GenerateTexture2D(uint8* ColoData, UTexture2D*& Texture2D);

	UFUNCTION()
	void RenderGrayScaleTexture();

	UFUNCTION()
	void RenderColorScaleTexture();

	UFUNCTION()
	double HandleHeightWithFilter(double Height) const;

	UFUNCTION()
	double ReverseHeight(double Height) const;

	UFUNCTION()
	FORCEINLINE double GetScaleCoordinateBasedLattice(double Origin, double MapSize, double LacSize, double Offset = 0.0, double Frequency = 1.0) const { return Origin * LacSize * Frequency / MapSize + Offset; }

	UFUNCTION()
	double NormalizePerlinNoiseValue(double Value) const ;

	UFUNCTION()
	double ApplyFBM(double X, double Y, double Offset);

	UFUNCTION()
	double NormdPDF(double X, double Mean, double StandardDeviation);
	
	UFUNCTION()
	FColor ApplyTerrainLayerColor(double Height) const;

	UFUNCTION()
	TArray<double> Conv2D(const TArray<double> &Kernel);

	//¾í»ý´¦Àí
	UFUNCTION()
	void AppplyConv2D();

	UFUNCTION()
	void InitKernel(TArray<double> &Kernel);

	UFUNCTION(BlueprintCallable)
	void DisplayTimeCostResult() const;

	UFUNCTION()
		void OnCoastErosionFinished(double InFinishTimeStamp);
private:
	
	TSharedPtr<class UPangolinHeightMap> HeightMapPtr;

	UPROPERTY(EditAnywhere, Category="HeightMap Settings")
	int32 HeightMapSize = HeightMapDefaultSize::Size_128;

	UPROPERTY()
	int32 TerrainSize;//Actually Terrain Size

	UPROPERTY(EditAnywhere, Category = "HeightMap Settings")
	int32 TerrainSeed = 312312;

	UPROPERTY(EditAnywhere, Category="HeightMap Settings", Meta = (ClampMin = "0.0", ClampMax = "1.0"))
	FVector2D HeightFilter = FVector2D(0., 1.);

	UPROPERTY(EditAnywhere, Category = "HeightMap Settings")
	TMap<ETerrainLayerType, FTerrainLayerInfo> LayerColorMaps;

	UPROPERTY(DuplicateTransient)
	TArray<double> HeightMap;

	UPROPERTY(EditAnywhere, Category = "HeightMap Settings")
	bool bIsTextureUpdate = true;

	UPROPERTY(EditAnywhere, Category = "HeightMap Settings")
	EHeightmapType HeightmapType = EHeightmapType::HMT_Perlin;

	UPROPERTY()
	UTexture2D* GrayScaleTexture;

	UPROPERTY()
	UTexture2D* ColorScaleTexture;

	UPROPERTY(EditAnywhere, Category = "Noise Settings", Meta = (ClampMin = "1", ClampMax = "64"))
	int32 LatticeSize = 4;

	UPROPERTY(EditAnywhere, Category = "Noise Settings")
	bool bShouldFBM = true;

	UPROPERTY(EditAnywhere, Category = "Noise Settings", Meta = (ClampMin = "2", ClampMax = "8", EditCondition = "bShouldFBM"))
	int32 Octaves = 4;

	UPROPERTY(EditAnywhere, Category = "Noise Settings", Meta = (ClampMin = "0.0", ClampMax = "1.0", EditCondition = "bShouldFBM"))
	double Persistance = 0.5;

	UPROPERTY(EditAnywhere, Category = "Noise Settings", Meta = (ClampMin = "1.0", EditCondition = "bShouldFBM"))
	double Lacunarity = 2.0;

	UPROPERTY(EditAnywhere, Category = "Fourier Transformation")
	bool bShouldConv = false;
	
	UPROPERTY(EditAnywhere, Category = "Fourier Transformation", Meta = (ClampMin = "1", EditCondition = "bShouldConv"))
	int32 ConvolutionKernelSize = 3;

	UPROPERTY(EditAnywhere, Category = "Fourier Transformation", Meta = (EditCondition = "bShouldConv"))
	EConvKernelType KernelType = EConvKernelType::EKT_GaussKernel;

	UPROPERTY(EditAnywhere, Category = "Erosion Settings")
	bool bShouldCoastErosion = true;

	UPROPERTY(EditAnywhere, Category = "Erosion Settings")
	bool bUseMultiThread = true;

	UPROPERTY(EditAnywhere, Category = "Erosion Settings", Meta = (EditCondition = "bShouldCoastErosion"))
	int32 CoastErosionTime = 10;

	UPROPERTY(EditAnywhere, Category = "Erosion Settings", Meta = (EditCondition = "bShouldCoastErosion"))
	ECoastErosionSimulateType CoastErosionSimulateType = ECoastErosionSimulateType::CES_Normal;

	UPROPERTY(EditAnywhere, Category = "Erosion Settings", Meta = (EditCondition = "bShouldCoastErosion"))
	TMap<EMapDirectionType, FCoastErosionInfo> CoastErosionMap;

	double StartTimeStamp;
	double FinishTimeStamp;
	int32 FinishCount = 0;

	TArray<double> TimeCostRecods;



	UPROPERTY(EditAnywhere, Category = "TerrainRender Settings")
	bool bRenderTerrain = true;

};
