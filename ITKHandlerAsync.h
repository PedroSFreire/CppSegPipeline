#ifndef ITKHandlerAsync_H
#define ITKHandlerAsync_H
#include "ITKHandler.h"
#include "GPUAnisotropic.h"
#include <opencv2/opencv.hpp>


class ITKHandlerAsync : public ITKHandler
{
	using ClampFilterType = itk::ClampImageFilter<ImageType, ImageType>;
	ClampFilterType::Pointer clampFilter;

	using CastFilterToFloatType = itk::CastImageFilter<ImageType, ImageFloatType>;
	CastFilterToFloatType::Pointer CastFilterToFloat;


	using CastFilterToIntType = itk::CastImageFilter<ImageFloatType, ImageType>;
	CastFilterToIntType::Pointer CastFilterToInt;


	using RescaleFilterType = itk::RescaleIntensityImageFilter<ImageType, ImageType>;
	RescaleFilterType::Pointer rescaleFilter;

public:
	ITKHandlerAsync();
	void clampSetup();
	void setupFilterPipeline(std::string filename);
	void runEqFilter(int* buffer, int sizeX, int sizeY, int sizeZ);
	void setupFilters(std::string filename);




};
#endif

