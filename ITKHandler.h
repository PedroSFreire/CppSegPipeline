#ifndef ITKHandler_H
#define ITKHandler_H
#include <string>
#include "ITKIncludes.h"
#include "ManualSobel.h"

class ITKHandler
{

public:

	using ReaderType = itk::ImageFileReader<ImageType>;
	ReaderType::Pointer reader;
	using ThresholdFilterType = itk::BinaryThresholdImageFilter<ImageType, ImageType>;
	ThresholdFilterType::Pointer airThresholdFilter;

	using ConnectedComponentFilterType = itk::ConnectedComponentImageFilter<ImageType, ImageType>;
	ConnectedComponentFilterType::Pointer ccFilter;
	using WriterType = itk::ImageFileWriter<ImageType>;
	WriterType::Pointer writer;
	using SobelOperatorType = itk::SobelOperator<INT32, 3>;
	using SobelFilterType = itk::NeighborhoodOperatorImageFilter<ImageType, ImageType>;
	SobelOperatorType sobelXOperator;
	SobelFilterType::Pointer sobelXFilter = SobelFilterType::New();
	SobelOperatorType sobelYOperator;
	SobelFilterType::Pointer sobelYFilter = SobelFilterType::New();
	SobelOperatorType sobelZOperator;
	SobelFilterType::Pointer sobelZFilter = SobelFilterType::New();

	using HistogramFilterType = itk::HistogramMatchingImageFilter<ImageType, ImageType>;
	HistogramFilterType::Pointer histogramFilter;


	using AnisotropicFilterType = itk::GradientAnisotropicDiffusionImageFilter<ImageFloatType, ImageFloatType>;
	AnisotropicFilterType::Pointer diffusionFilter;

	//using AnisotropicFilterType = itk::GPUGradientAnisotropicDiffusionImageFilter<ImageFloatType, ImageFloatType>;
	//AnisotropicFilterType::Pointer diffusionFilter;



	using ClampFilterType = itk::ClampImageFilter<ImageType, ImageType>;
	ClampFilterType::Pointer clampFilter;

	using CastFilterToFloatType = itk::CastImageFilter<ImageType, ImageFloatType>;
	CastFilterToFloatType::Pointer CastFilterToFloat;


	using CastFilterToIntType = itk::CastImageFilter<ImageFloatType, ImageType>;
	CastFilterToIntType::Pointer CastFilterToInt;


	using RescaleFilterType = itk::RescaleIntensityImageFilter<ImageType, ImageType>;
	RescaleFilterType::Pointer rescaleFilter;

	ITKHandler();
	void setupReader(std::string filename);
	int getObjCount();
	void setupWriter(std::string filename);
	void setupThresholdFilter();
	void setupCCL();
	void setupYSobel();
	void setupFilters(std::string filename);
	void setupPipeline();
	void runPipelineAir();
	void runPipelineFlats(ImageType::Pointer img);
	void clampSetup();
	void setupFilterPipeline(std::string filename);

	// **Step 2: Histogram Normalization**
	void histogramFilterSetup(ImageType::Pointer& image);

	// **Step 3: Anisotropic Diffusion**
	void anisotropicSetup();
};
#endif
