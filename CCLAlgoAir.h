#ifndef CCLAlgoAir_H
#define CCLAlgoAir_H
#include <string>
#include <stack>
#include "ITKIncludes.h"
#include "CCLAlgoBase.h"
class CCLAlgoAir : public CCLAlgoBase
{
public:
	

	
	CCLAlgoAir(int* imageBuffer, int count, int x, int y, int z);
	//void runCCL();
	void removeUnwantedAirCC(int minSize);
	//void removeSmallCC(int minSize);
	//void removeCC(int id);
	//int  getIdFromPos(int x, int y, int z);
	//void expandLabels();
	void addValidCC(std::vector<int>, int* finalBuffer);
	void run();
};
#endif

