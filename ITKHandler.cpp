
#include "ITKHandler.h"
#include <iostream>





//builder
ITKHandler::ITKHandler() {
	itk::NiftiImageIOFactory::RegisterOneFactory();
}
void ITKHandler::setupReader(std::string filename) {
	reader = ReaderType::New();
	reader->SetFileName(filename);
}
void ITKHandler::setupWriter(std::string filename) {
	writer = WriterType::New();
	writer->SetFileName("Seg" + filename);
}


void ITKHandler::setupThresholdFilter() {


	thresholdFilter = ThresholdFilterType::New();

	//Threshold filter setup
	thresholdFilter->SetInput(reader->GetOutput());
	thresholdFilter->SetUpperThreshold(-600);
	thresholdFilter->SetInsideValue(1);
	thresholdFilter->SetOutsideValue(0);

}

void ITKHandler::setupCCL() {
	ccFilter = ConnectedComponentFilterType::New();
}
void ITKHandler::setupSmallCCRemove() {
	relabelFilter = RelabelFilterType::New();
	relabelFilter->SetMinimumObjectSize(5000);
}

void ITKHandler::setupFilters(std::string filename) {
	itk::NiftiImageIOFactory::RegisterOneFactory();
	setupReader(filename);
	setupWriter(filename);
	setupThresholdFilter();
	setupCCL();
	setupSmallCCRemove();
}

void ITKHandler::setupPipeline() {
	// Pipeline setup
	thresholdFilter->SetInput(reader->GetOutput());
	//ccFilter->SetInput(thresholdFilter->GetOutput());
	//relabelFilter->SetInput(ccFilter->GetOutput());
	//writer->SetInput(relabelFilter->GetOutput());

}

void ITKHandler::runPipeline() {
	// Run the pipeline
	try {
		//writer->Update();
		thresholdFilter->Update();
	}
	catch (itk::ExceptionObject& err) {
		std::cerr << "Error writing file: " << err << std::endl;
	}
}
