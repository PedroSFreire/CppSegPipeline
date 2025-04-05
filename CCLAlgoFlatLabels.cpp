#include "CCLAlgoFlatLabels.h"

#include "omp.h"



	CCLAlgoFlatLabels::CCLAlgoFlatLabels(int* image, int count, int x, int y, int z,int* ogBuffer): CCLAlgoBase(imageBuffer, count, x, y, z)
	{
		imageBuffer = image;
		objCount = count;
		xSize = x;
		ySize = y;
		zSize = z;
		volume = xSize * ySize * zSize;
		ccVec.resize(objCount+1);

		ogImgBuffer = ogBuffer;


	}

	


	void CCLAlgoFlatLabels::expandLabels() {
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


	void CCLAlgoFlatLabels::addValidCC(std::vector<int> validCC, int* finalBuffer) {
		for (int i = 0; i < objCount; i++) {
			if (ccVec[i].size == 0)
				continue;
			if (validCC[i] == 1) {
				for (int j = 0; j < ccVec[i].size; j++)
					finalBuffer[ccVec[i].voxels[j]] = 1;
	
			}
		}
	}



	void  CCLAlgoFlatLabels::expandVoxel(int id, std::vector<int>& voxels, int* finalBuffer,int yMax,int yMin, int* airBuffer) {
		ManualSobel* sobelOp = new ManualSobel();
		int x, y, z;
		idToPos(&x, &y, &z, id, xSize, ySize, zSize);
		if (y < ySize - 1 && y<yMax) {
			int aboveVoxel = getIdFromPos(x, y + 1, z);
			int aboveSobel = sobelOp->getSobelY(airBuffer, x, y + 1, z, xSize, ySize, zSize);
			if ( aboveSobel < 0 && imageBuffer[aboveVoxel]==0) {
				finalBuffer[aboveVoxel] = 10;
				voxels.push_back(aboveVoxel);
				imageBuffer[aboveVoxel] = 10;
				expandVoxelUpwards(aboveVoxel, voxels, finalBuffer, yMax, yMin, airBuffer);
			}
		}


		if (y > 1 && y > yMin ) {
			int belowVoxel = getIdFromPos(x, y - 1, z);
			int belowSobel = sobelOp->getSobelY(airBuffer, x, y - 1, z, xSize, ySize, zSize);
			if ( belowSobel < 0 && imageBuffer[belowVoxel] == 0) {
				finalBuffer[belowVoxel] = 10;
				voxels.push_back(belowVoxel);
				imageBuffer[belowVoxel] = 10;
				expandVoxelUpwards(belowVoxel, voxels, finalBuffer, yMax, yMin, airBuffer);
			}
		}

		if (x > 1) {
			int leftVoxel = getIdFromPos(x - 1, y, z);
			int leftSobel = sobelOp->getSobelY(airBuffer, x - 1, y, z, xSize, ySize, zSize);
			if ( leftSobel < 0 && imageBuffer[leftVoxel] == 0) {
				finalBuffer[leftVoxel] = 10;
				voxels.push_back(leftVoxel);
				imageBuffer[leftVoxel] = 10;
				expandVoxelUpwards(leftVoxel, voxels, finalBuffer, yMax, yMin, airBuffer);
			}
		}

		if (x < xSize - 1) {
			int rightVoxel = getIdFromPos(x + 1, y, z);
			int rightSobel = sobelOp->getSobelY(airBuffer, x + 1, y, z, xSize, ySize, zSize);
			if ( rightSobel < 0 && imageBuffer[rightVoxel] == 0) {
				finalBuffer[rightVoxel] = 10;
				voxels.push_back(rightVoxel);
				imageBuffer[rightVoxel] = 10;
				expandVoxelUpwards(rightVoxel, voxels, finalBuffer, yMax, yMin, airBuffer);
			}
		}


		if (z < zSize - 1) {
			int frontVoxel = getIdFromPos(x, y, z + 1);
			int frontSobel = sobelOp->getSobelY(airBuffer, x, y, z + 1, xSize, ySize, zSize);
			if ( frontSobel < 0 && imageBuffer[frontVoxel] == 0) {
				finalBuffer[frontVoxel] = 10;
				voxels.push_back(frontVoxel);
				imageBuffer[frontVoxel] = 10;
				expandVoxelUpwards(frontVoxel, voxels, finalBuffer, yMax, yMin, airBuffer);
			}
		}

		if (z > 1) {
			int backVoxel = getIdFromPos(x, y, z - 1);
			int backSobel = sobelOp->getSobelY(airBuffer, x, y, z - 1, xSize, ySize, zSize);
			if  (backSobel < 0 && imageBuffer[backVoxel] == 0) {
				finalBuffer[backVoxel] = 10;
				voxels.push_back(backVoxel);
				imageBuffer[backVoxel] = 10;
				expandVoxelUpwards(backVoxel, voxels, finalBuffer, yMax, yMin, airBuffer);
			}
		}



	}


	void CCLAlgoFlatLabels::expandVoxelUpwards(int id, std::vector<int> voxels, int* finalBuffer, int yMax, int yMin, int* airBuffer) {
		// check if voxel intensity is increasing as we go down and if so expando to it
		int x, y, z;
		idToPos(&x, &y, &z, id, xSize, ySize, zSize);

		int bellowId = getIdFromPos(x,y + 1,z);
		int bellowId2 = getIdFromPos(x, y+2, z );
		int bellowId3 = getIdFromPos(x, y+3, z );
		int bellowId4 = getIdFromPos(x, y + 4, z);

		if(finalBuffer[bellowId2] == 1)
			finalBuffer[bellowId] = 50;

		if (finalBuffer[bellowId3] == 1) {
			finalBuffer[bellowId] = 50;
			finalBuffer[bellowId2] = 50;
		}

		if (finalBuffer[bellowId4] == 1) {
			finalBuffer[bellowId] = 50;
			finalBuffer[bellowId2] = 50;
			finalBuffer[bellowId3] = 50;
		}


		imageBuffer[bellowId] = 1;
		imageBuffer[bellowId2] = 1;
		imageBuffer[bellowId3] = 1;
		/*if (ogImgBuffer[id] < ogImgBuffer[bellowId] && ogImgBuffer[bellowId] < ogImgBuffer[bellowId2] && finalBuffer[bellowId] == 0) {
			finalBuffer[bellowId] = 1;
			voxels.push_back(bellowId);
			imageBuffer[bellowId] = 1;
		}*/

	}

	void  CCLAlgoFlatLabels::regionExpansion(int id, int* finalBuffer, int* airBuffer) {
		std::vector<int> voxels;
		int counts = 0;
		int posY_max = 0;
		int posY_min = 100000;
		for (int voxel = 0; voxel < ccVec[id].size; voxel++) {
			int x, y, z;
			idToPos(&x, &y, &z, ccVec[id].voxels[voxel], xSize, ySize, zSize);
			if (y< posY_min)
				posY_min = y;
			if (y > posY_max)
				posY_max = y;
			voxels.push_back(ccVec[id].voxels[voxel]);
			
		}
		posY_max = +LabelExpandBottom;
		posY_max -= LabelExpandTop;
		while (voxels.size()) {
			int voxel = voxels.back();
			voxels.pop_back();
			expandVoxel(voxel, voxels,finalBuffer, posY_max, posY_min,airBuffer);
			expandVoxelUpwards(voxel, voxels, finalBuffer, posY_max, posY_min, airBuffer);
		}

	}

	void CCLAlgoFlatLabels::labelFinalExpansion(std::vector<int> validCC, int* finalBuffer, int* airBuffer) {
		for (int i = 0; i < objCount; i++) {
			if (ccVec[i].size == 0)
				continue;
			if (validCC[i] == 1) {
				regionExpansion(i,finalBuffer, airBuffer);
				
			}
		}
	}


	void CCLAlgoFlatLabels::run() {
		runCCL();
		removeSmallCC(FlatSizeMin);

	}




