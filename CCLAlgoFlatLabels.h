#ifndef CCLAlgoFlats_H
#define CCLAlgoFlats_H
#include <string>
#include <stack>
#include "ITKIncludes.h"
#include "CCLAlgoBase.h"
#include "ManualSobel.h"
class CCLAlgoFlatLabels : public CCLAlgoBase
{
public:
	
	int* ogImgBuffer;


	CCLAlgoFlatLabels(int* imageBuffer,int count, int x, int y, int z,int* ogBuffer);
	//void runCCL();
	//void removeUnwantedAirCC(int minSize);
	//void removeSmallCC(int minSize);
	//void removeCC(int id);
	//int  getIdFromPos(int x, int y, int z);
	void expandLabels();
	void addValidCC(std::vector<int>,int* finalBuffer);
	void labelFinalExpansion(std::vector<int> validCC, int* finalBuffer, int* airBuffer);
	void regionExpansion(int id, int* finalBuffer, int* airBuffer);
	void expandVoxel(int id, std::vector<int> &voxels, int* finalBuffer, int yMax, int yMin, int* airBuffer);
	void expandVoxelUpwards(int id, std::vector<int> voxels, int* finalBuffer, int yMax, int yMin, int* airBuffer);

	void run();
};
#endif

