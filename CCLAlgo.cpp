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
		for (int i = 0; i < objCount; i++) {
			if (ccVec[i].size < minSize) {
				removeCC(i);
				ccVec[i].size = 0;
			}
		}

	}



	void CCLAlgo::expandLabels() {
//#pragma omp parallel for num_threads(16)
		for (int i = 0; i < objCount ; i++) {
			if (ccVec[i].size == 0)
				continue;
			std::vector<int> voxels;
			int counts=0;
			for( int voxel = 0; voxel< ccVec[i].size;voxel++) {
				int x, y, z;
				idToPos(&x, &y, &z, ccVec[i].voxels[voxel], xSize, ySize, zSize);
				int aboveVoxel = getIdFromPos(x, y + 1, z);
				int belowVoxel = getIdFromPos(x, y - 1, z);
				if(z>1 && z< zSize-1)
					if (imageBuffer[aboveVoxel] == 0) {
						imageBuffer[aboveVoxel] = i;
						++counts;
						voxels.push_back(aboveVoxel);
					}
				if (z > 1 && z < zSize - 1)
					if (imageBuffer[belowVoxel] == 0) {
						imageBuffer[belowVoxel] = i;
						++counts;
						voxels.push_back(belowVoxel);
					}
			}
//#pragma omp critical
			ccVec[i].voxels.insert(ccVec[i].voxels.end(), voxels.begin(), voxels.end());
//#pragma omp critical
			ccVec[i].size += counts;
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
