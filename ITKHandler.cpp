
#include "ITKHandler.h"
#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>
#include "GPUAnisotropic.h"
#include <opencv2/opencv.hpp>

//builder
ITKHandler::ITKHandler() {
	itk::NiftiImageIOFactory::RegisterOneFactory();
}
void ITKHandler::setupReader(std::string filename) {
	reader = ReaderType::New();
	reader->SetFileName(filename);
	readerOg = ReaderType::New();
	readerOg->SetFileName(filename);
	readerOg->Update();
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


	liquidThresholdFilter = ThresholdFilterType::New();

	//Threshold filter setup

	liquidThresholdFilter->SetLowerThreshold(210);
	liquidThresholdFilter->SetInsideValue(1);
	liquidThresholdFilter->SetOutsideValue(0);
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



float computeWeight(float center, float neighbor) {
	float diff = center - neighbor;
	return std::exp(-diff * diff);
}



float apply3DFilter(float* buffer, int x, int y, int z, int width, int height, int depth) {
	float center = buffer[getIdFromPos(x, y, z, width, height)];
	float sum = 0.0f;
	float weightTotal = 0.0f;

	for (int dx = -1; dx <= 1; ++dx) {
		for (int dy = -1; dy <= 1; ++dy) {
			for (int dz = -1; dz <= 1; ++dz) {
				int nx = x + dx;
				int ny = y + dy;
				int nz = z + dz;

				// Ensure the neighbor coordinates are within bounds
				if (nx >= 0 && nx < width && ny >= 0 && ny < height && nz >= 0 && nz < depth) {
					float neighbor = buffer[getIdFromPos(nx, ny, nz, width, height)];
					float weight = computeWeight(center, neighbor);
					sum += neighbor * weight;
					weightTotal += weight;
				}
			}
		}
	}

	return (weightTotal > 0.0f) ? (sum / weightTotal) : center;
}



float apply3DMedianFilter(float* buffer, int x, int y, int z, int width, int height, int depth) {
	std::vector<float> neighbors;

	for (int dx = -1; dx <= 1; ++dx) {
		for (int dy = -1; dy <= 1; ++dy) {
			for (int dz = -1; dz <= 1; ++dz) {
				int nx = x + dx;
				int ny = y + dy;
				int nz = z + dz;

				// Ensure the neighbor coordinates are within bounds
				if (nx >= 0 && nx < width && ny >= 0 && ny < height && nz >= 0 && nz < depth) {
					neighbors.push_back(buffer[getIdFromPos(nx, ny, nz, width, height)]);
				}
			}
		}
	}

	// Sort the neighbors to find the median
	std::sort(neighbors.begin(), neighbors.end());

	// Return the median value
	size_t medianIndex = (neighbors.size() / 2)+2;
	return neighbors[medianIndex]; // For odd sizes, this is the middle; for even sizes, you can average.
}


void runAniFilter(float* buffer, int xSize, int ySize, int zSize) {
#pragma omp parallel for num_threads(16)
	for (int i = 0; i < xSize; i++) {
		for (int j = 0; j < ySize; j++) {
			for (int k = 0; k < zSize; k++) {
				buffer[getIdFromPos(i,j,k,xSize,ySize)]= apply3DMedianFilter(buffer, i, j, k, xSize, ySize,zSize);
			}
		}
	}
}





void ITKHandler::runPipelineFlats(ImageType::Pointer img) {
	ImageType::Pointer solImg = ImageType::New();
	ManualSobel *sobelOp = new ManualSobel();
	int* buffer = img->GetBufferPointer();
	solImg->SetRegions(img->GetLargestPossibleRegion());
	//solImg->SetSpacing(img->GetSpacing());       // Copy voxel size
	solImg->SetOrigin(img->GetOrigin());         // Copy world-space origin
	//solImg->SetDirection(img->GetDirection());
	solImg->Allocate();
	solImg->FillBuffer(0);
	int* sol = solImg->GetBufferPointer();
	std::vector<int> down{ 0,-1,0 };


	
	itk::Size<3> imgSize = img->GetLargestPossibleRegion().GetSize();
	int xSize = imgSize[0], ySize = imgSize[1], zSize = imgSize[2];


	for (int k = 0; k < zSize; k++) {
		for (int j = 0; j < ySize; j++) {
			for (int i = 0; i < xSize; i++) {
				int idx = getIdFromPos(i, j, k, xSize, ySize);
				//int aboveIDX = getIdFromPos(i+1, j, k, xSize, ySize);
				//int belowIDX = getIdFromPos(i - 1, j, k, xSize, ySize);
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

					if (std::inner_product(std::begin(sobel), std::end(sobel), std::begin(down), 0) > 0.4 && length > 0 && sobel[0] < 0.3 && sobel[0] >-0.3 && sobel[2] < 0.3 && sobel[2] >-0.3) {
						int topId = getIdFromPos(i, j + 1, k, xSize, ySize);
						int botId = getIdFromPos(i, j - 1, k, xSize, ySize);
						int botId2 = getIdFromPos(i, j + 2, k, xSize, ySize);
						int botId3 = getIdFromPos(i, j + 3, k, xSize, ySize);
						int botId4 = getIdFromPos(i, j + 4, k, xSize, ySize);

						sol[topId] = 1;
						sol[botId2] = 1;
						//sol[botId3] = 1;




						sol[idx] = 1;
					}
					
				}
				
			}
		}
	}

	ccFilter->SetInput(solImg);
	ccFilter->Update();
}




void ITKHandler::clampSetup() {
	clampFilter = ClampFilterType::New();

	int window_Level = 40;
	int window_Width = 400;
	int lowerBound = window_Level - window_Width / 2; 
	int upperBound = window_Level + window_Width / 2; 

	clampFilter->SetBounds(lowerBound, upperBound);
}

// **Step 2: Histogram Normalization**
void ITKHandler::histogramFilterSetup(ImageType::Pointer& image) {
	histogramFilter = HistogramFilterType::New();

	histogramFilter->SetReferenceImage(image);  // Self-reference for normalization
	histogramFilter->SetNumberOfHistogramLevels(256);
	histogramFilter->SetNumberOfMatchPoints(128);
	histogramFilter->ThresholdAtMeanIntensityOn();

}

// **Step 3: Anisotropic Diffusion**
void ITKHandler::anisotropicSetup() {
	diffusionFilter = AnisotropicFilterType::New();
	diffusionFilter->SetNumberOfIterations(15);  // niter=15
	diffusionFilter->SetTimeStep(0.04);          // gamma=0.12
	diffusionFilter->SetConductanceParameter(55); // kappa=55


}


void ITKHandler::setupFilters(std::string filename) {
	itk::NiftiImageIOFactory::RegisterOneFactory();
	setupReader(filename);
	setupWriter(filename);
	CastFilterToFloat = CastFilterToFloatType::New();
	CastFilterToInt = CastFilterToIntType::New();
	rescaleFilter = RescaleFilterType::New();
	rescaleFilter->SetOutputMinimum(0.0);
	rescaleFilter->SetOutputMaximum(255.0);
	ImageType::Pointer img;
	reader->Update();
	img = reader->GetOutput();
	clampSetup();
	histogramFilterSetup(img);
	anisotropicSetup();
	setupCCL();
	setupYSobel();
	setupThresholdFilter();
}



void runEqFilter(int* buffer, int sizeX, int sizeY, int sizeZ) {
	for (int k = 0; k < sizeZ; k++) {
		int histogram[256] = { 0 };
		for (int i = 0; i < sizeX; ++i)
			for (int j = 0; j < sizeY; ++j)
				histogram[buffer[getIdFromPos(i, j, k, sizeX, sizeY) ]]++;
		
		int cdf[256] = { 0 };
		cdf[0] = histogram[0];
		for (int i = 1; i < 256; ++i)
			cdf[i] = cdf[i - 1] + histogram[i];
		int minCdf = *std::min_element(cdf, cdf + 256);  // Find first non-zero CDF value
		int totalPixels = sizeY * sizeY;
		int lookupTable[256];
		

		for (int i = 0; i <256; ++i)
			lookupTable[i] = ((cdf[i] - minCdf) * 255) / (totalPixels - minCdf);

		for (int i = 0; i < sizeX; ++i)
			for (int j = 0; j < sizeY; ++j)
				buffer[getIdFromPos(i, j, k, sizeX, sizeY)] = lookupTable[buffer[getIdFromPos(i, j, k, sizeX, sizeY)]];


	}
}


void ITKHandler::setupFilterPipeline(std::string filename) {
	// Pipeline setup

	ImageType::Pointer image;
	ImageFloatType::Pointer floatImg;

	//run clamp histogram and rescale filters
	clampFilter->SetInput(reader->GetOutput());
	rescaleFilter->SetInput(clampFilter->GetOutput());
	rescaleFilter->Update();
	image = rescaleFilter->GetOutput();

	// get image size and setup for gpu
	itk::Size<3> imgSize = image->GetLargestPossibleRegion().GetSize();
	GPUHandler GPUH = GPUHandler(imgSize[0], imgSize[1], imgSize[2]);


	//Custom histigram normalization
	runEqFilter(image->GetBufferPointer(), imgSize[0], imgSize[1], imgSize[2]);


	//convert to float for better ani filter may not be required but it is fast enouph to not matter
	CastFilterToFloat->SetInput(image);
	CastFilterToFloat->Update();
	floatImg = CastFilterToFloat->GetOutput();



	//gpu anisotropic diffusion
	GPUH.run(floatImg->GetBufferPointer());


	//convert back to int for ccl
	CastFilterToInt->SetInput(floatImg);
	CastFilterToInt->Update();

	//threshold and ccl
	liquidThresholdFilter->SetInput(CastFilterToInt->GetOutput());
	ccFilter->SetInput(liquidThresholdFilter->GetOutput());
	ccFilter->Update();

}