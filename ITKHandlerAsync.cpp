#include "ITKHandlerAsync.h"



ITKHandlerAsync::ITKHandlerAsync() : ITKHandler() {
	itk::NiftiImageIOFactory::RegisterOneFactory();
}

void ITKHandlerAsync::clampSetup() {
	clampFilter = ClampFilterType::New();

	int window_Level = 40;
	int window_Width = 400;
	int lowerBound = window_Level - window_Width / 2;
	int upperBound = window_Level + window_Width / 2;

	clampFilter->SetBounds(lowerBound, upperBound);
}

void ITKHandlerAsync::setupFilters(std::string filename) {
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
	setupCCL();
	setupThresholdFilter();
}







void ITKHandlerAsync::runEqFilter(int* buffer, int sizeX, int sizeY, int sizeZ) {
	for (int k = 0; k < sizeZ; k++) {
		int histogram[256] = { 0 };
		for (int i = 0; i < sizeX; ++i)
			for (int j = 0; j < sizeY; ++j)
				histogram[buffer[getIdFromPos(i, j, k, sizeX, sizeY)]]++;

		int cdf[256] = { 0 };
		cdf[0] = histogram[0];
		for (int i = 1; i < 256; ++i)
			cdf[i] = cdf[i - 1] + histogram[i];
		int minCdf = *std::min_element(cdf, cdf + 256);  // Find first non-zero CDF value
		int totalPixels = sizeY * sizeY;
		int lookupTable[256];


		for (int i = 0; i < 256; ++i)
			lookupTable[i] = ((cdf[i] - minCdf) * 255) / (totalPixels - minCdf);

		for (int i = 0; i < sizeX; ++i)
			for (int j = 0; j < sizeY; ++j)
				buffer[getIdFromPos(i, j, k, sizeX, sizeY)] = lookupTable[buffer[getIdFromPos(i, j, k, sizeX, sizeY)]];


	}
}


void ITKHandlerAsync::setupFilterPipeline(std::string filename) {
	// Pipeline setup
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
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

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Async stack = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[µs]" << std::endl;
}