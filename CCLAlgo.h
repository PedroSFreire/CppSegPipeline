#ifndef CCLAlgo_H
#define CCLAlgo_H
#include <string>
#include <stack>
#include "ITKIncludes.h"
class CCLAlgo
{
public:
	struct ccData {
		int size=0;
		std::vector<int> voxels;
	};

	int   objCount;
	int* imageBuffer;
	int xSize, ySize, zSize;
	std::vector<ccData> ccVec;
	std::vector<ccData> f_ccVec;

	


	CCLAlgo(int* imageBuffer,int count, int x, int y, int z);
	void runCCL();
	void removeUnwantedAirCC(int minSize);
	void removeSmallCC(int minSize);
	void removeCC(int id);
	int  getIdFromPos(int x, int y, int z);
};
#endif

