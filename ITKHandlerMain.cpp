#include "ITKHandlerMain.h"





ITKHandlerMain::ITKHandlerMain(): ITKHandler() {
	itk::NiftiImageIOFactory::RegisterOneFactory();
}

void ITKHandlerMain::setupFilters(std::string filename) {
	itk::NiftiImageIOFactory::RegisterOneFactory();
	setupReader(filename);
	setupWriter(filename);
	ImageType::Pointer img;
	reader->Update();
	img = reader->GetOutput();
	setupCCL();
	setupThresholdFilter();
}


void ITKHandlerMain::runPipelineAir() {
	// Run the pipeline
	try {

		ccFilter->Update();
	}
	catch (itk::ExceptionObject& err) {
		std::cerr << "Error writing file: " << err << std::endl;
	}
}


void ITKHandlerMain::runPipelineFlats(ImageType::Pointer img) {
	ImageType::Pointer solImg = ImageType::New();
	ManualSobel* sobelOp = new ManualSobel();
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
				int sobelYV = sobelOp->getSobelY(buffer, i, j, k, xSize, ySize, zSize);
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

