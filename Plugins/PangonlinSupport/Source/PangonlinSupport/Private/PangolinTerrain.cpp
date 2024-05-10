// Fill out your copyright notice in the Description page of Project Settings.


#include "PangolinTerrain.h"

DEFINE_LOG_CATEGORY(LogHeightmap);

UPangolinHeightMap::UPangolinHeightMap(int32 Size)
{
	Data.Reserve(Size);
	TerrainSize = Size;
}

void UPangolinHeightMap::SetHeightMap(const TArray<double>& InData)
{
	Data = InData;
}

void UPangolinHeightMap::SetTerrainSize(int32 Size)
{
	TerrainSize = Size;
	Data.Empty();
	Data.Reserve(Size * Size);
}

bool UPangolinHeightMap::CheckElement(int32 Index)
{
	return Data.IsValidIndex(Index);
}

double UPangolinHeightMap::GetElement(int32 Index)
{
	return Data[Index];
}

void UPangolinHeightMap::SetElement(int32 Index, double Value)
{
	Data[Index]	 = Value;
}



UPangolinTerrain::UPangolinTerrain()
{
	LayerColorMaps.Add(TPair<ETerrainLayerType, FTerrainLayerInfo>
		(ETerrainLayerType::ETT_DeepWater, FTerrainLayerInfo(0.25, FColor(0,51, 102))));
	LayerColorMaps.Add(TPair<ETerrainLayerType, FTerrainLayerInfo>
		(ETerrainLayerType::ETT_Water, FTerrainLayerInfo(0.4, FColor(0, 0, 204))));
	LayerColorMaps.Add(TPair<ETerrainLayerType, FTerrainLayerInfo>
		(ETerrainLayerType::ETT_Sand, FTerrainLayerInfo(0.45, FColor(204, 204, 0))));
	LayerColorMaps.Add(TPair<ETerrainLayerType, FTerrainLayerInfo>
		(ETerrainLayerType::ETT_GrassBottom, FTerrainLayerInfo(0.6, FColor(0, 102, 0))));
	LayerColorMaps.Add(TPair<ETerrainLayerType, FTerrainLayerInfo>
		(ETerrainLayerType::ETT_Grass, FTerrainLayerInfo(0.7, FColor(0, 51, 0))));
	LayerColorMaps.Add(TPair<ETerrainLayerType, FTerrainLayerInfo>
		(ETerrainLayerType::ETT_Rock, FTerrainLayerInfo(0.9, FColor(102, 0, 0))));
	LayerColorMaps.Add(TPair<ETerrainLayerType, FTerrainLayerInfo>
		(ETerrainLayerType::ETT_Mountain, FTerrainLayerInfo(1.0, FColor(51, 0, 0))));

	CoastErosionMap.Add(TPair<EMapDirectionType, FCoastErosionInfo>(EMapDirectionType::MDT_East, FCoastErosionInfo()));
	CoastErosionMap.Add(TPair<EMapDirectionType, FCoastErosionInfo>(EMapDirectionType::MDT_West, FCoastErosionInfo()));
	CoastErosionMap.Add(TPair<EMapDirectionType, FCoastErosionInfo>(EMapDirectionType::MDT_North, FCoastErosionInfo()));
	CoastErosionMap.Add(TPair<EMapDirectionType, FCoastErosionInfo>(EMapDirectionType::MDT_South, FCoastErosionInfo()));

	HeightMapPtr = MakeShared<UPangolinHeightMap>(HeightMapSize);
}





void UPangolinTerrain::GenerateHeightMapByPerlin()
{
	HeightMap.Empty();
	HeightMap.Reserve(HeightMapSize * HeightMapSize);
	int32 MurMurRandom = MurmurFinalize32(TerrainSeed) % 100;
	double RandomOffset = LatticeSize * FMath::Sin((double)MurMurRandom *PI/180.0);
	int32 Offset = 30;
	for (int32 y = 0; y < HeightMapSize; y++)
	{
		for (int32 x = 0; x < HeightMapSize; x++)
		{
			double Height = 0.;
			if (!bShouldFBM)
			{
				double ScaledX = GetScaleCoordinateBasedLattice(x, HeightMapSize, LatticeSize, RandomOffset);
				double ScaledY = GetScaleCoordinateBasedLattice(y, HeightMapSize, LatticeSize, RandomOffset);
				Height =  NormalizePerlinNoiseValue(FMath::PerlinNoise2D(FVector2D(ScaledX, ScaledY)));
			}
			else
			{
				Height = NormalizePerlinNoiseValue(ApplyFBM(x, y, RandomOffset));
			}
			HeightMap.Add(HandleHeightWithFilter(Height));

		}
	}
}

void UPangolinTerrain::SetHeightMapSize(int32 InSize)
{
	HeightMapSize = InSize;
}

void UPangolinTerrain::SetHeightMapElement(int32 Index, double Value)
{
	if (HeightMap.IsValidIndex(Index))
	{
		HeightMap[Index] = Value;
	}
	else
	{
		UE_LOG(LogHeightmap, Error, TEXT("Invaild Index %d of heightmap"), Index);
	}

}

double UPangolinTerrain::GetHeightMapElement(int32 Index, bool bShowLogInfo) const
{
	if (HeightMap.IsValidIndex(Index))
	{
		return HeightMap[Index];
	}
	else
	{
		if (bShowLogInfo)
		{
			UE_LOG(LogHeightmap, Error, TEXT("Invaild Index %d of heightmap"), Index);
		}
		return 0.;
	}
}

TArray<double> UPangolinTerrain::GetNoise1D(int32 Index, bool IsHorizontal) const
{
	if (Index>=TerrainSize || Index < 0)
	{
		UE_LOG(LogHeightmap, Error, TEXT("Invalid Index: %d, out of range [0, %d)"), Index, TerrainSize);
		return TArray<double>();
	}
	TArray<double> Line;
	int32 BeginPos = 0;
	if (IsHorizontal)
	{
		BeginPos = Index * TerrainSize;
	}
	else
	{
		BeginPos = Index;
	}
	for (int32 ItemIndex = 0; ItemIndex < TerrainSize; ItemIndex++)
	{
		int32 Pos = IsHorizontal ? (BeginPos + ItemIndex) : (BeginPos + TerrainSize * ItemIndex);

		if (HeightMap.IsValidIndex(Pos))
		{
			Line.Add(HeightMap[Pos]);
		}
		else
		{
			UE_LOG(LogHeightmap, Warning, TEXT("Calculated position %d is out of bounds"), Pos);
			return TArray<double>();

		}
	}

	return Line;
}

void UPangolinTerrain::GenerateHeightMapByDiamondSquare()
{
	HeightMap.Empty();
	int32 ChunkSize = HeightMapSize + 1;
	HeightMap.Reserve(ChunkSize * ChunkSize);
	HeightMap.Init(0., ChunkSize * ChunkSize);

	FRandomStream RandStream(TerrainSeed);
	HeightMap[FindIndex(0, 0)] = FMath::RandRange(0., 1.);
	HeightMap[FindIndex(ChunkSize - 1, 0)] = FMath::RandRange(0., 1.);
	HeightMap[FindIndex(0, ChunkSize - 1)] = FMath::RandRange(0., 1.);
	HeightMap[FindIndex(ChunkSize - 1, ChunkSize - 1)] = FMath::RandRange(0., 1.);

	double H = 0.75;
	for (int32 Width = ChunkSize - 1; Width >= 2; Width /= 2, H *= 0.5) {
		int32 Half = Width / 2;


		for (int32 x = 0; x <= ChunkSize - 1 - Width; x += Width) {
			for (int32 y = 0; y <= ChunkSize - 1 - Width; y += Width) {
				double Average = 0.25 * (HeightMap[FindIndex(x, y)] + HeightMap[FindIndex(x + Width, y)] + HeightMap[FindIndex(x, y + Width)] + HeightMap[FindIndex(x + Width, y + Width)]);
				HeightMap[FindIndex(x + Half, y + Half)] = Average + (FMath::RandRange(-1., 1.) * 2 * H) + H;


			}
		}

		for (int32 x = Half; x <= ChunkSize - 1 - Half; x += Half) {
			for (int32 y = Half; y <= ChunkSize - 1 - Half; y += Half) {

				HeightMap[FindIndex(x - Half, y)] = (HeightMap[FindIndex(x - Half, y + Half)] + HeightMap[FindIndex(x - Half, y - Half)] + HeightMap[FindIndex(x, y)]) / 3.0 + (FMath::RandRange(-1., 1.) * 2 * H) + H;
				HeightMap[FindIndex(x + Half, y)] = (HeightMap[FindIndex(x + Half, y - Half)] + HeightMap[FindIndex(x + Half, y + Half)] + HeightMap[FindIndex(x, y)]) / 3.0 + (FMath::RandRange(-1., 1.) * 2 * H) + H;
				HeightMap[FindIndex(x, y - Half)] = (HeightMap[FindIndex(x - Half, y - Half)] + HeightMap[FindIndex(x + Half, y - Half)] + HeightMap[FindIndex(x, y)]) / 3.0 + (FMath::RandRange(-1., 1.) * 2 * H) + H;
				HeightMap[FindIndex(x, y + Half)] = (HeightMap[FindIndex(x - Half, y + Half)] + HeightMap[FindIndex(x + Half, y + Half)] + HeightMap[FindIndex(x, y)]) / 3.0 + (FMath::RandRange(-1., 1.) * 2 * H) + H;

			}
		}
	}
}

void UPangolinTerrain::CoastErosion()
{

	StartTimeStamp = FDateTime::Now().GetTimeOfDay().GetTotalSeconds();
	FinishTimeStamp = 0.;
	if (bUseMultiThread)
	{
		for (const auto& Item : CoastErosionMap)
		{
			if (!Item.Value.bDisable)
			{
				UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EMapDirectionType"), true);

				FString Str(EnumPtr->GetNameByValue((int)Item.Key).ToString());
				FCoastErosionRunnable* Runnable = new FCoastErosionRunnable(this, Str, Item.Key, Item.Value.ErosionUnitNum, CoastErosionSimulateType, CoastErosionTime);
				Runnable->CoastErosionAgendaFinished.BindUObject(this, &UPangolinTerrain::OnCoastErosionFinished);

				FRunnableThread* RunnableThread4 = FRunnableThread::Create(Runnable, *Runnable->ThreadName);
			}
		}

		FPlatformProcess::Sleep(0.05);
	}
	else
	{
		for (const auto& Item : CoastErosionMap)
		{
			if (!Item.Value.bDisable)
			{
				UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EMapDirectionType"), true);

				FString Str(EnumPtr->GetNameByValue((int)Item.Key).ToString());
				FCoastErosionRunnable* Runnable = new FCoastErosionRunnable(this, Str, Item.Key, Item.Value.ErosionUnitNum, CoastErosionSimulateType, CoastErosionTime);
				Runnable->Run();
			}
		}
		FinishTimeStamp = FDateTime::Now().GetTimeOfDay().GetTotalSeconds();
		double Cost = FinishTimeStamp - StartTimeStamp;
		UE_LOG(LogTemp, Error, TEXT("Coast Erosion cost :%f s"), Cost);
		TimeCostRecods.Add(Cost);
		DisplayTimeCostResult();
	}

}

void UPangolinTerrain::GenerateHeightMap()
{
	switch (HeightmapType)
	{
	case EHeightmapType::HMT_Perlin:
		GenerateHeightMapByPerlin();
		break;
	case EHeightmapType::HMT_DiamondSquare:
		GenerateHeightMapByDiamondSquare();
		break;
	default:
		break;
	}
	TerrainSize = FMath::Sqrt((double)HeightMap.Num());
	if (bShouldCoastErosion)
	{
		CoastErosion();
	}

	if (bShouldConv)
	{
		AppplyConv2D();
	}

	if (bIsTextureUpdate)
	{
		RenderNoiseTexture();
	}
}

bool UPangolinTerrain::IsSeaCell(int32 X, int32 Y) const
{
	return  GetHeightMapElement(FindIndex(X, Y)) < 0.4;
}

bool UPangolinTerrain::IsSeaCell(int32 Index) const
{
	return GetHeightMapElement(Index) < 0.4;
}

bool UPangolinTerrain::IsValidPos(int32 X, int32 Y) const
{
	if (X >= 0 && Y >= 0 && X < TerrainSize && Y < TerrainSize)
	{
		int32 Index = X + Y * TerrainSize;
		return HeightMap.IsValidIndex(Index);
	}
	else
	{
		return false;
	}
}

bool UPangolinTerrain::IsValidIndex(int32 Index) const
{
	return HeightMap.IsValidIndex(Index);
}

int32 UPangolinTerrain::FindIndex(int32 X, int32 Y, bool bShowLogInfo) const
{
	int32 Index = X + Y * TerrainSize;
	if (!HeightMap.IsValidIndex(Index))
	{
		if (bShowLogInfo)
		{
			UE_LOG(LogHeightmap, Warning, TEXT("Calculated position %d is out of bounds, ( %d, %d)"), Index, X, Y);
		}
		return -1;
	}
	return Index;
}

FIntPoint UPangolinTerrain::FindPosByIndex(int32 Index) const
{
	return FIntPoint(Index % TerrainSize, Index / TerrainSize);
}

double UPangolinTerrain::HandleHeightWithFilter(double Height) const
{
	if (Height < 0.)
	{
		return FMath::Abs(Height);
	}
	if (Height < HeightFilter.X || Height > HeightFilter.Y)
	{
		return  Height * (HeightFilter.Y - HeightFilter.X);
	}
	return Height;
}

double UPangolinTerrain::ReverseHeight(double Height) const
{
	return 1.0 - Height;
}

void UPangolinTerrain::GenerateTexture2D(uint8* ColorData, UTexture2D*& Texture2D)
{
	Texture2D = nullptr;
	Texture2D = NewObject<UTexture2D>();
	uint8 PixelSize = sizeof(uint8) * 4;
	const uint32 TextureDataSize = TerrainSize * TerrainSize * PixelSize;
	Texture2D->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
	Texture2D->Source.Init(TerrainSize , TerrainSize, 1, 1, ETextureSourceFormat::TSF_BGRA8);
	uint8* SourceData = Texture2D->Source.LockMip(0);
	FMemory::Memcpy(SourceData, ColorData, TextureDataSize);
	Texture2D->Source.UnlockMip(0);


	Texture2D->SetPlatformData(new FTexturePlatformData());
	Texture2D->GetPlatformData()->SizeX = TerrainSize;
	Texture2D->GetPlatformData()->SizeY = TerrainSize;
	Texture2D->GetPlatformData()->SetNumSlices(1);
	Texture2D->GetPlatformData()->PixelFormat = PF_B8G8R8A8;

	FTexture2DMipMap* Mip = new FTexture2DMipMap();
	Texture2D->GetPlatformData()->Mips.Add(Mip);
	Mip->SizeX = TerrainSize;
	Mip->SizeY = TerrainSize;
	Mip->BulkData.Lock(LOCK_READ_WRITE);

	uint8* TextureData = (uint8*)Mip->BulkData.Realloc(TerrainSize * TerrainSize * 4);

	FMemory::Memcpy(TextureData, ColorData, TextureDataSize);
	Mip->BulkData.Unlock();

	Texture2D->UpdateResource();

}

void UPangolinTerrain::RenderNoiseTexture()
{
	int32 ElementNumber = HeightMap.Num();
	TArray<uint8> GrayScaleColorData;
	TArray<uint8> ColorScaleColorData;
	for (int32 i = 0; i <  ElementNumber; i++)
	{
		double Height = FMath::Clamp(HeightMap[i], 0., 1.0);
		int32 GrayScale = 255 * Height;
		FColor Color = ApplyTerrainLayerColor(Height);

		{
			GrayScaleColorData.Add(GrayScale);
			GrayScaleColorData.Add(GrayScale);
			GrayScaleColorData.Add(GrayScale);
			GrayScaleColorData.Add(255);
		}

		{
			ColorScaleColorData.Add(Color.B);
			ColorScaleColorData.Add(Color.G);
			ColorScaleColorData.Add(Color.R);
			ColorScaleColorData.Add(255);
		}
	}
	GenerateTexture2D(GrayScaleColorData.GetData(), this->GrayScaleTexture);
	GenerateTexture2D(ColorScaleColorData.GetData(), this->ColorScaleTexture);
}

void UPangolinTerrain::RenderGrayScaleTexture()
{
	int32 ElementNumber = HeightMap.Num();
	TArray<uint8> ColorData;
	ColorData.Reserve(ElementNumber * 4);
	for (int32 i = 0; i < ElementNumber; i++)
	{
		int32 GrayScale = 255 * HeightMap[i];
		ColorData.Add(GrayScale);
		ColorData.Add(GrayScale);
		ColorData.Add(GrayScale);
		ColorData.Add(255);

	}
	GenerateTexture2D(ColorData.GetData(), this->GrayScaleTexture);
}

void UPangolinTerrain::RenderColorScaleTexture()
{
	int32 ElementNumber = HeightMap.Num();
	TArray<uint8> ColorData;
	ColorData.Reserve(ElementNumber * 4);

	for (int32 i = 0; i < ElementNumber; i++)
	{
		FColor Color = ApplyTerrainLayerColor(HeightMap[i]);
		ColorData.Add(Color.B);
		ColorData.Add(Color.G);
		ColorData.Add(Color.R);
		ColorData.Add(255);
	}
	GenerateTexture2D(ColorData.GetData(), this->ColorScaleTexture);
}

double UPangolinTerrain::NormalizePerlinNoiseValue(double Value) const
{
	return 0.5 + 0.5 * Value;
}

double UPangolinTerrain::ApplyFBM(double X, double Y, double Offset)
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

double UPangolinTerrain::NormdPDF(double X, double Mean, double StandardDeviation)
{
	if (FMath::IsNearlyZero(StandardDeviation))
	{
		StandardDeviation = 0.01;
	}
	const double D = (X - Mean) / StandardDeviation;
	return  1.0/(FMath::Sqrt(2*PI)* StandardDeviation)* FMath::Exp(-0.5 * D * D);
}


FColor UPangolinTerrain::ApplyTerrainLayerColor(double Height) const
{
	for (const auto& LayerItem : LayerColorMaps)
	{
		FTerrainLayerInfo LayerInfo = LayerItem.Value;
		if (Height < LayerInfo.UpperBound)
		{
			return LayerInfo.LayerColor;
		}
	}
	return FColor::White;
}

TArray<double> UPangolinTerrain::Conv2D(const TArray<double>& Kernel)
{
	int32 KernelSize = Kernel.Num();
	int32 OriginSize = HeightMap.Num();

	if (KernelSize <= 0)
	{
		UE_LOG(LogHeightmap, Error, TEXT("KernelSize is zero"));
		return TArray<double>();
	}

	if (OriginSize - KernelSize < 0)
	{
		UE_LOG(LogHeightmap, Warning, TEXT("The kernel size is larger than heighmap size"));
	}

	int32 KernelN = FMath::Sqrt((double)KernelSize);
	int32 OriginN = FMath::Sqrt((double)(OriginSize));
	TArray<double> Res;
	int32 ResN = OriginN - KernelN + 1;
	Res.Reserve(ResN * ResN);
	for (int i = 0; i < ResN * ResN; i++)
	{
		Res.Add(0.);
	}

	for (int y = 0; y <= OriginN - KernelN; y++)
	{
		for (int x = 0; x <= OriginN - KernelN; x++)
		{
			double Sum = 0.;
			for (int i = 0; i < KernelN; i++)
			{
				for (int j = 0; j < KernelN; j++)
				{
					int32 KernelIndex = j + i * KernelN;
					Sum += Kernel[KernelIndex] * HeightMap[FindIndex(x + j, y + i)];

				}
			}
			Res[(x + y * ResN)] = Sum;

		}
	}
	return Res;
}

void UPangolinTerrain::AppplyConv2D()
{
	TArray<double> Kernel;
	InitKernel(Kernel);
	TArray<double> Res = Conv2D(Kernel);
	if (Res.Num() > 0)
	{
		int32 Size = FMath::Sqrt((double)Res.Num());
		TerrainSize = Size;
		HeightMap = Res;
	}
}

void UPangolinTerrain::InitKernel(TArray<double>& Kernel)
{
	if (!(ConvolutionKernelSize & 1))
	{
		ConvolutionKernelSize++;
	}
	Kernel.Empty();
	int32 N = ConvolutionKernelSize * ConvolutionKernelSize;
	Kernel.Reserve(N);
	switch (KernelType)
	{
	case EConvKernelType::None:
		Kernel.Init(0., N);
		Kernel[ConvolutionKernelSize / 2 + ConvolutionKernelSize * (ConvolutionKernelSize / 2)] = 1.;
		break;
	case EConvKernelType::EKT_MeanKernel:
		for (int32 i = 0; i < N; i++)
		{
			Kernel.Add(1.0 / (double)N);
		}
		break;
	case EConvKernelType::EKT_GaussKernel:
		int32 Sigma = 1;
		Kernel.Init(0.f, N);

		int32 Center = ConvolutionKernelSize / 2;

		// Precompute the constant factor of the Gaussian function
		double Constant = 1.f / (2.f * PI * FMath::Square(Sigma));

		double Sum = 0.f;
		for (int32 i = 0; i < ConvolutionKernelSize; ++i)
		{
			for (int32 j = 0; j < ConvolutionKernelSize; ++j)
			{
				int32 X = i - Center;
				int32 Y = j - Center;
				float Value = Constant * FMath::Exp((double)(-(FMath::Square(X) + FMath::Square(Y)) / (2 * FMath::Square(Sigma))));
				Kernel[i * ConvolutionKernelSize + j] = Value;
				Sum += Value;
			}
		}

		for (int32 i = 0; i < N; i++)
		{
			Kernel[i] = Kernel[i] / Sum;
		}
		break;

	}
}

void UPangolinTerrain::DisplayTimeCostResult() const
{
	int32 Num = TimeCostRecods.Num();
	if (Num <= 0)
	{
		return;
	}
	double Sum = 0.;
	for (const auto Item : TimeCostRecods)
	{
		Sum += Item;
	}
	UE_LOG(LogTemp, Error, TEXT("%d times average cost:%f s"), Num, Sum / Num);
}

void UPangolinTerrain::OnCoastErosionFinished(double InFinishTimeStamp)
{
	FinishCount++;
	FinishTimeStamp = FMath::Max(InFinishTimeStamp, FinishTimeStamp);
	if (FinishCount == 4)
	{
		FinishCount = 0;
		double Cost = FinishTimeStamp - StartTimeStamp;
		UE_LOG(LogTemp, Error, TEXT("Coast Erosion cost :%f s"), Cost);
		TimeCostRecods.Add(Cost);
		double Sum = 0.;
		DisplayTimeCostResult();
	}
}
