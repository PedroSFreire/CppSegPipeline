#include "CCLAlgoAir.h"

#include "omp.h"



	CCLAlgoAir::CCLAlgoAir(int* image, int count, int x, int y, int z) : CCLAlgoBase(imageBuffer, count, x, y, z)
	{
		imageBuffer = image;
		objCount = count;
		xSize = x;
		ySize = y;
		zSize = z;

		ccVec.resize(objCount+1);


	}


	void CCLAlgoAir::removeUnwantedAirCC(int minSize) {
		removeSmallCC(minSize);
		std::vector<int> count( objCount,0);
		for (int i = 0; i < xSize; i++) {
			for (int j = 0; j < ySize; j++) {
				int id = getIdFromPos(i, j, 0);

				if (imageBuffer[id] > 0) {
					count[imageBuffer[id]]++;
					//removeCC(imageBuffer[id]);
					//ccVec[imageBuffer[id]].size = 0;
				}
			}
		}

		for (int i = 0; i < objCount; i++) {
			if (count[i] > 10) {
				removeCC(i);
				ccVec[i].size = 0;
			}
		}
		for (int i = 0; i < xSize; i++) {
			for (int j = 0; j < ySize; j++) {
				int id = getIdFromPos(i, j, zSize-1);
				if (imageBuffer[id] > 0) {
					removeCC(imageBuffer[id]);
					ccVec[imageBuffer[id]].size = 0;
				}
			}
		}
		for (int i = 0; i < objCount+1; i++) {
			if(ccVec[i].size>0)
				f_ccVec.push_back(ccVec[i]);
		}



	}

	void CCLAlgoAir::addValidCC(std::vector<int> validCC, int* finalBuffer) {
		for (int i = 0; i < objCount; i++) {
			if (ccVec[i].size == 0)
				continue;
			if (validCC[i] == 1) {
				for (int j = 0; j < ccVec[i].size; j++)
					finalBuffer[ccVec[i].voxels[j]] = 1;
			}
		}
	}
