#include "CCLAlgoBase.h"

#include "omp.h"



	CCLAlgoBase::CCLAlgoBase(int* image,int count ,int x, int y, int z)
	{
		imageBuffer = image;
		objCount = count;
		xSize = x;
		ySize = y;
		zSize = z;

		ccVec.resize(objCount);


	}
	void CCLAlgoBase::idToPos(int* x, int* y, int* z, int id, int xSize, int ySize, int zSize) {
		*z = id / (ySize * xSize);
		*y = (id % (ySize * xSize)) / xSize;
		*x = id % xSize;
	}	

	int CCLAlgoBase::getIdFromPos(int x, int y, int z) {
		return z * ySize * xSize + y * xSize + x;
	}



	void CCLAlgoBase::runCCL() {
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

	void CCLAlgoBase::removeCC(int id) {

		for (int i = 0; i < ccVec[id].size; i++) {
			imageBuffer[ccVec[id].voxels[i]] = 0;
		}
	}

	void CCLAlgoBase::removeSmallCC(const int minSize) {
		for (int i = 0; i < objCount; i++) {
			if (ccVec[i].size < minSize) {
				removeCC(i);
				ccVec[i].size = 0;
			}
		}

	}

	void CCLAlgoBase::removeBigCC(int maxSize) {
		for (int i = 0; i < objCount; i++) {
			if (ccVec[i].size > maxSize) {
				removeCC(i);
				ccVec[i].size = 0;
			}
		}

	}


