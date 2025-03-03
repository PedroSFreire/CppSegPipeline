#include "CCLAlgo.h"

#include "omp.h"



	CCLAlgo::CCLAlgo(int* image,int count ,int x, int y, int z)
	{
		imageBuffer = image;
		objCount = count;
		xSize = x;
		ySize = y;
		zSize = z;

		ccVec.resize(objCount+1);


	}
	void idToPos(int* x, int* y, int* z, int id, int xSize, int ySize, int zSize) {
		*z = id / (ySize * xSize);
		*y = (id % (ySize * xSize)) / xSize;
		*x = id % xSize;
	}	

	int CCLAlgo::getIdFromPos(int x, int y, int z) {
		return z * ySize * xSize + y * xSize + x;
	}




	void CCLAlgo::runCCL() {
		// CCL Algorithm
		int id, val;


		for (int i = 0; i < xSize; i++) {
			for (int j = 0; j < ySize; j++) {
				for (int k = 0; k < zSize; k++) {

					id = getIdFromPos(i, j, k);
					
					val = imageBuffer[id];

					++ccVec[val].size;
					ccVec[val].voxels.push_back(id);
				}
			}
		}

	}

	void CCLAlgo::removeCC(int id) {

		for (int i = 0; i < ccVec[id].size; i++) {
			imageBuffer[ccVec[id].voxels[i]] = 0;
		}
	}

	void CCLAlgo::removeSmallCC(int minSize) {
		for (int i = 0; i < objCount + 1; i++) {
			if (ccVec[i].size < minSize) {
				removeCC(i);
				ccVec[i].size = 0;
			}
		}

	}


	void CCLAlgo::removeUnwantedAirCC(int minSize) {
		removeSmallCC(minSize);
		for (int i = 0; i < xSize; i++) {
			for (int j = 0; j < ySize; j++) {
				int id = getIdFromPos(i, j, 0);

				if (imageBuffer[id] > 0) {
					removeCC(imageBuffer[id]);
					ccVec[imageBuffer[id]].size = 0;
				}
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
