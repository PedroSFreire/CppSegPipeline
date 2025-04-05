
#include "ITKHandler.h"
#include <iostream>
#include <vector>
#include <omp.h>


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
	airThresholdFilter->SetUpperThreshold(AirThreshold);
	airThresholdFilter->SetInsideValue(1);
	airThresholdFilter->SetOutsideValue(0);


	liquidThresholdFilter = ThresholdFilterType::New();

	//Threshold filter setup

	liquidThresholdFilter->SetLowerThreshold(LiquidThreshold);
	liquidThresholdFilter->SetInsideValue(1);
	liquidThresholdFilter->SetOutsideValue(0);
}

void ITKHandler::setupCCL() {
	ccFilter = ConnectedComponentFilterType::New();
}

void ITKHandler::setupPipeline() {
	// Pipeline setup
	airThresholdFilter->SetInput(reader->GetOutput());
	ccFilter->SetInput(airThresholdFilter->GetOutput());
}

int ITKHandler::getObjCount() {
	return ccFilter->GetObjectCount();
}



int ITKHandler::getIdFromPos(int x, int y, int z,int xSize, int ySize) {
	return z * ySize * xSize + y * xSize + x;
}








//Here is the inital cpu filter code that was used to test the anisotropic diffusion filter. Now it uses gpu.

/*float computeWeight(float center, float neighbor) {
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
}*/



/*float apply3DMedianFilter(float* buffer, int x, int y, int z, int width, int height, int depth) {
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
}*/


/*void runAniFilter(float* buffer, int xSize, int ySize, int zSize) {
#pragma omp parallel for num_threads(16)
	for (int i = 0; i < xSize; i++) {
		for (int j = 0; j < ySize; j++) {
			for (int k = 0; k < zSize; k++) {
				buffer[getIdFromPos(i,j,k,xSize,ySize)]= apply3DMedianFilter(buffer, i, j, k, xSize, ySize,zSize);
			}
		}
	}
}*/









