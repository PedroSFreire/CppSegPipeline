#include "CCLAlgoLiquidPockets.h"

#include "omp.h"



	CCLAlgoLiquidPockets::CCLAlgoLiquidPockets(int* image,int count ,int x, int y, int z) : CCLAlgoBase(imageBuffer, count, x, y, z)
	{
		imageBuffer = image;
		objCount = count;
		xSize = x;
		ySize = y;
		zSize = z;
		volume = xSize * ySize * zSize;
		finalImg = ImageType::New();

		ccVec.resize(objCount+1);


	}

	void CCLAlgoLiquidPockets::intersectLabels(int* labelBuffer, int* airBuffer,int labelCount,int AirCount, std::vector<ccData> flatLabels, itk::ImageRegion<3U> region) {
		flatLabelsValid = std::vector<int>(labelCount, 0);
		airLabelsValid =  std::vector<int>(AirCount, 0);
		liquidLabelsValid = std::vector<int>(objCount, 0);

		finalImg->SetRegions(region);
		finalImg->Allocate();
		finalImg->FillBuffer(0);
		int* finalBuffer = finalImg->GetBufferPointer();
		for (int flatId = 0; flatId < labelCount; flatId++) {
			for (int j = 0; j < flatLabels[flatId].size; j++) {
				int idx = flatLabels[flatId].voxels[j];
				if (imageBuffer[idx] > 0 && labelBuffer[idx] > 0) {
					flatLabelsValid[labelBuffer[idx]] = 1;
					liquidLabelsValid[imageBuffer[idx]] = 1;
					finalBuffer[idx] = 1;
				}
					
			}
		}

		for (int flatId = 0; flatId < labelCount; flatId++) {
			if (flatLabelsValid[flatId] == 1) {
				for (int j = 0; j < flatLabels[flatId].size; j++) {
					int idx = flatLabels[flatId].voxels[j];
					if (airBuffer[idx] > 0 && labelBuffer[idx] > 0) {
						airLabelsValid[airBuffer[idx]] = 1;
						finalBuffer[idx] = 1;
					}
				}
			}
		}




	}

	void CCLAlgoLiquidPockets::addValidCC() {
		int* finalBuffer = finalImg->GetBufferPointer();
		for (int i = 0; i < objCount; i++) {
			if (ccVec[i].size == 0)
				continue;
			if (liquidLabelsValid[i] == 1) {
				for (int j = 0; j < ccVec[i].size; j++)
					finalBuffer[ccVec[i].voxels[j]] = 1;
			}
		}
	}




	void CCLAlgoLiquidPockets::liquidFinalExpansion(  int* buffer) {
		int* finalBuffer = finalImg->GetBufferPointer();
		for (int j = ySize - 1; j > 0; j--) {
			for (int k = 0; k < zSize; k++) {
				for (int i = 0; i < xSize; i++) {
					int idx = getIdFromPos(i, j, k);
					int topId = getIdFromPos(i, j - 1, k );
					if (buffer[topId] > -500 && finalBuffer[idx] > 0)
						finalBuffer[idx] = 1;
				}
			}
		}
	}


	void CCLAlgoLiquidPockets::run() {
		runCCL();
		removeSmallCC(LiquidSizeMin * volume);
		removeBigCC(0.02 * xSize * ySize * zSize);

	}
	
