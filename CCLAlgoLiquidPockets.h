#ifndef CCLAlgo_H
#define CCLAlgo_H
#include <string>
#include <stack>
#include "ITKIncludes.h"
#include "CCLAlgoBase.h"
class CCLAlgoLiquidPockets : public CCLAlgoBase
{
public:
	
	ImageType::Pointer finalImg;
	std::vector<int> flatLabelsValid;
	std::vector<int> airLabelsValid;
	std::vector<int> liquidLabelsValid;

	CCLAlgoLiquidPockets(int* imageBuffer,int count, int x, int y, int z);
	void addValidCC();
	void intersectLabels(int* labelBuffer, int* airBuffer, int labelCount, int airCount, std::vector<ccData> flatLabels,itk::ImageRegion<3U> region);
	void liquidFinalExpansion(int* airBuffer);
	void run();
};
#endif

