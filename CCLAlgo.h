#ifndef CCLAlgo_H
#define CCLAlgo_H
#include <string>
#include <stack>
#include "ITKIncludes.h"
class CCLAlgo
{
public:
	struct ccData {
		int label;
		int size=0;
		std::vector<int> voxels;
	};

	int* imageBuffer;
	int xSize, ySize, zSize;
	std::vector<ccData*> ccVec;
	


	CCLAlgo(int* imageBuffer, int x, int y, int z);
	void runCCL();
	void addVoxelToCC(int id, int minVal);
	void runSmallCCRemove(int minSize);
	int  getIdFromPos(int x, int y, int z);
	int  firstPass();
};
#endif

