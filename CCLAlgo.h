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
		std::stack<int> voxels;
	};

	int* imageBuffer;
	int xSize, ySize, zSize;
	std::unordered_map<int, ccData*> ccMap;

	


	CCLAlgo(int* imageBuffer, int x, int y, int z);
	void runCCL();
	void addVoxelToCC(int id, int minVal);
	void runSmallCCRemove(int minSize);
	int  getIdFromPos(int x, int y, int z);
	int  firstPass();
};
#endif

