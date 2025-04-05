#ifndef CCLAlgoBase_H
#define CCLAlgoBase_H
#include <string>
#include <stack>
#include "ITKIncludes.h"
#include "defines.h"
class CCLAlgoBase 
{
public:
	struct ccData {
		int size=0;
		std::vector<int> voxels;
	};
	int volume;
	int   objCount;
	int* imageBuffer;
	int xSize, ySize, zSize;
	std::vector<ccData> ccVec;
	std::vector<ccData> f_ccVec;

	


	CCLAlgoBase(int* imageBuffer,int count, int x, int y, int z);
	void runCCL();
	void idToPos(int* x, int* y, int* z, int id, int xSize, int ySize, int zSize);
	void removeSmallCC(const  int minSize);
	void removeBigCC(int maxSize);
	void removeCC(int id);
	int  getIdFromPos(int x, int y, int z);


};
#endif

