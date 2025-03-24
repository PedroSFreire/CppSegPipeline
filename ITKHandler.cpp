
#include "ITKHandler.h"
#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>

#include <opencv2/opencv.hpp>

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
	std::cout << "time to sobel = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[탎]" << std::endl;

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
	diffusionFilter->SetNumberOfIterations(2);  // niter=15
	diffusionFilter->SetTimeStep(0.04);          // gamma=0.12
	diffusionFilter->SetConductanceParameter(60); // kappa=55


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
	writer->SetFileName("FilterStuff" + filename);
	reader->Update();
	image = reader->GetOutput();
	itk::Size<3> imgSize = image->GetLargestPossibleRegion().GetSize();
	clampFilter->SetInput(image);
	rescaleFilter->SetInput(clampFilter->GetOutput());


	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	rescaleFilter->Update();
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "clamp time = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[탎]" << std::endl;


	image = rescaleFilter->GetOutput();
	histogramFilter->SetInput(image);
	writer->SetFileName("rescaledmStuff" + filename);
	writer->SetInput(image);
	writer->Update();
	begin = std::chrono::steady_clock::now();
	//histogramFilter->Update();
	runEqFilter(image->GetBufferPointer(), imgSize[0], imgSize[1], imgSize[2]);
	end = std::chrono::steady_clock::now();
	std::cout << "Histogram time = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[탎]" << std::endl;


	//image = histogramFilter->GetOutput();
	
	writer->SetFileName("posthistogramStuff" + filename);
	writer->SetInput(image);
	writer->Update();


	CastFilterToFloat->SetInput(image);

	CastFilterToFloat->Update();

	begin = std::chrono::steady_clock::now();
	CastFilterToFloat->Update();
	end = std::chrono::steady_clock::now();
	std::cout << "int to float  = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[탎]" << std::endl;

	floatImg = CastFilterToFloat->GetOutput();

	diffusionFilter->SetInput(floatImg);


	begin = std::chrono::steady_clock::now();
	diffusionFilter->Update();
	//for(int rep = 0;rep<=2;rep++)
	//	runAniFilter(floatImg->GetBufferPointer(), imgSize[0], imgSize[1], imgSize[2]);

	end = std::chrono::steady_clock::now();
	std::cout << "difussion  = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[탎]" << std::endl;

	floatImg = diffusionFilter->GetOutput();

	CastFilterToInt->SetInput(floatImg);


	begin = std::chrono::steady_clock::now();
	CastFilterToInt->Update();
	end = std::chrono::steady_clock::now();
	std::cout << "to float = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[탎]" << std::endl;
	writer->SetFileName("FilterStuff" + filename);
	writer->SetInput(CastFilterToInt->GetOutput());
	writer->Update();
}