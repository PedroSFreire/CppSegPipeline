#ifndef ITKHandler_H
#define ITKHandler_H
#include <string>
#include "ITKIncludes.h"
#include "ManualSobel.h"
#include <chrono>
#include "defines.h"

class ITKHandler
{

public:

	using ReaderType = itk::ImageFileReader<ImageType>;
	ReaderType::Pointer reader;
	ReaderType::Pointer readerOg;
	using ThresholdFilterType = itk::BinaryThresholdImageFilter<ImageType, ImageType>;
	ThresholdFilterType::Pointer airThresholdFilter;
	ThresholdFilterType::Pointer liquidThresholdFilter;

	using ConnectedComponentFilterType = itk::ConnectedComponentImageFilter<ImageType, ImageType>;
	ConnectedComponentFilterType::Pointer ccFilter;
	using WriterType = itk::ImageFileWriter<ImageType>;
	WriterType::Pointer writer;






	ITKHandler();
	void setupReader(std::string filename);
	int getObjCount();
	void setupWriter(std::string filename);
	void setupThresholdFilter();
	void setupCCL();
	void setupPipeline();
	int getIdFromPos(int x, int y, int z, int xSize, int ySize);

};
#endif
