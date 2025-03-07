
#include "ITKHandler.h"
#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>



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


	airThresholdFilter = ThresholdFilterType::New();

	//Threshold filter setup
	airThresholdFilter->SetInput(reader->GetOutput());
	airThresholdFilter->SetUpperThreshold(-600);
	airThresholdFilter->SetInsideValue(1);
	airThresholdFilter->SetOutsideValue(0);
}

void ITKHandler::setupCCL() {
	ccFilter = ConnectedComponentFilterType::New();
}
void ITKHandler::setupYSobel() {
	//X sobel
	sobelXOperator.SetDirection(0);
	sobelXOperator.CreateDirectional();
	sobelXFilter->SetOperator(sobelXOperator);

	//Y sobel
	sobelYOperator.SetDirection(1); 
	sobelYOperator.CreateDirectional();
	sobelYFilter->SetOperator(sobelYOperator);
	
	//Z sobel
	sobelZOperator.SetDirection(2);
	sobelZOperator.CreateDirectional();
	sobelZFilter->SetOperator(sobelZOperator);
}

void ITKHandler::setupFilters(std::string filename) {
	itk::NiftiImageIOFactory::RegisterOneFactory();
	setupReader(filename);
	setupWriter(filename);
	
	setupCCL();
	setupYSobel();
	setupThresholdFilter();
}

void ITKHandler::setupPipeline() {
	// Pipeline setup
	airThresholdFilter->SetInput(reader->GetOutput());
	ccFilter->SetInput(airThresholdFilter->GetOutput());
}

int ITKHandler::getObjCount() {
	return ccFilter->GetObjectCount();
}

void ITKHandler::runPipelineAir() {
	// Run the pipeline
	try {

		ccFilter->Update();
	}
	catch (itk::ExceptionObject& err) {
		std::cerr << "Error writing file: " << err << std::endl;
	}
}

int getIdFromPos(int x, int y, int z,int xSize, int ySize) {
	return z * ySize * xSize + y * xSize + x;
}

int sobelX(ImageType::Pointer img, int x,int y,int z) {
	return 1;
}

int sobelZ(ImageType::Pointer img, int x, int y, int z) {
	return 1;
}



void ITKHandler::runPipelineFlats(ImageType::Pointer img) {
	ImageType::Pointer solImg = ImageType::New();
	ManualSobel *sobelOp = new ManualSobel();
	int* buffer = img->GetBufferPointer();
	solImg->SetRegions(img->GetLargestPossibleRegion());
	solImg->Allocate();
	solImg->FillBuffer(0);
	int* sol = solImg->GetBufferPointer();
	std::vector<int> down{ 0,-1,0 };


	
	itk::Size<3> imgSize = img->GetLargestPossibleRegion().GetSize();
	int xSize = imgSize[0], ySize = imgSize[1], zSize = imgSize[2];
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
#pragma omp parallel for num_threads(16)
	for (int i = 0; i < xSize; i++) {
		for (int j = 0; j < ySize; j++) {
			for (int k = 0; k < zSize; k++) {
				int idx = getIdFromPos(i, j, k, xSize, ySize);
				int aboveIDX = getIdFromPos(i+1, j, k, xSize, ySize);
				int belowIDX = getIdFromPos(i - 1, j, k, xSize, ySize);
				//if (buffer[idx] == 0 && buffer[aboveIDX] == 0 && buffer[belowIDX] == 0)
				//	continue;
				int sobelYV = sobelOp->getSobelY(buffer,i,j,k, xSize, ySize, zSize);
				if (sobelYV < 0) {
					int sobelXV = sobelOp->getSobelX(buffer, i, j, k, xSize, ySize, zSize);
					int sobelZV = sobelOp->getSobelZ(buffer, i, j, k, xSize, ySize, zSize);
					std::vector<int> sobel = { sobelXV , sobelYV , sobelZV };
					int length = std::inner_product(std::begin(sobel), std::end(sobel), std::begin(sobel), 0);
					length = sqrt(length);
					if (length > 0)
						sobel = { sobelXV / length , sobelYV / length , sobelZV / length };

					if (std::inner_product(std::begin(sobel), std::end(sobel), std::begin(down), 0) > 0.7 && length > 0) {

						sol[idx] = 1;
					}
					else sol[idx] = 0;
				}
				else {
					sol[idx] = 0;
				}
			}
		}
	}
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "time to sobel = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[µs]" << std::endl;

		ccFilter->SetInput(solImg);
		ccFilter->Update();
}
