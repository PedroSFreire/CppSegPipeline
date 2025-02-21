
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
	ThresholdFilterType::Pointer thresholdFilter;
	using ConnectedComponentFilterType = itk::ConnectedComponentImageFilter<ImageType, ImageType>;
	ConnectedComponentFilterType::Pointer ccFilter;
	using WriterType = itk::ImageFileWriter<ImageType>;
	WriterType::Pointer writer;
	using RelabelFilterType = itk::RelabelComponentImageFilter<ImageType, ImageType>;
	RelabelFilterType::Pointer relabelFilter;


	ITKHandler();
	void setupReader(std::string filename);
	void setupWriter(std::string filename);
	void setupThresholdFilter();
	void setupCCL();
	void setupSmallCCRemove();
	void setupFilters(std::string filename);
	void setupPipeline();
	void runPipeline();
};
#endif
