#ifndef ITKHandler_H
#define ITKHandler_H
#include <string>
#include "ITKIncludes.h"

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
};
#endif
