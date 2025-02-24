#include "CCLAlgo.h"





	CCLAlgo::CCLAlgo(int* image,int x, int y, int z)
	{
		imageBuffer = image;
		xSize = x;
		ySize = y;
		zSize = z;


	}


	int CCLAlgo::getIdFromPos(int x, int y, int z) {
		return x * ySize * zSize + y * zSize + z;
	}

	void CCLAlgo::addVoxelToCC(int id, int minVal) {

		ccVec[minVal-1]->voxels.push_back(id);
		ccVec[minVal - 1]->size++;

	}

	int CCLAlgo::firstPass() {
		int id;
		bool connected = false;
		int label = 0;
		int topId, topValue, leftId, leftValue, frontId, frontValue;

		for (int i = 0; i < xSize; i++) {
			connected = false;
			for (int j = 0; j < ySize; j++) {
				connected = false;
				for (int k = 0; k < zSize; k++) {
					id = getIdFromPos(i, j, k);
					if (imageBuffer[id] == 1 && connected) {
						// Do CCL
						if (i > 0) {
							topId = getIdFromPos(i - 1, j, k);
							topValue = imageBuffer[topId];
							if (topValue == 0)
								topValue = xSize * ySize * zSize;
						}
						else topValue = xSize * ySize * zSize;

						if (j > 0) {
							leftId = getIdFromPos(i, j - 1, k);
							leftValue = imageBuffer[leftId];
							if (leftValue == 0)
								leftValue = xSize * ySize * zSize;
						}
						else leftValue = xSize * ySize * zSize;

						if (k > 0) {
							frontId = getIdFromPos(i, j, k - 1);
							frontValue = imageBuffer[frontId];
							if (frontValue == 0)
								frontValue = xSize * ySize * zSize;
						}
						else frontValue = xSize * ySize * zSize;




						int minVal = std::min(std::min(topValue, leftValue), frontValue);

						//add voxel to apropriate component
						addVoxelToCC(id, minVal);
						imageBuffer[id] = minVal;

					}
					else if (imageBuffer[id] == 1 && !connected) {
						// New label
						label++;
						connected = true;
						struct ccData* newCC = new ccData;
						newCC->label = label;
						newCC->voxels.push_back(id);
						newCC->size++;
						ccVec.push_back(newCC);

					}
					else {
						connected = false;
					}
				}
			}
		}
		return label;
	}


	void CCLAlgo::runCCL() {
		// CCL Algorithm
		int label = firstPass();

	}
