#ifndef CCLAlgoFinal_H
#define CCLAlgoFinal_H
#include <string>
#include <stack>
#include "ITKIncludes.h"
#include "CCLAlgoBase.h"
class CCLAlgoFinal : public CCLAlgoBase
{
public:
	

	
	CCLAlgoFinal(int* imageBuffer, int count, int x, int y, int z);


	int findFinalComponent();
	void run();

};
#endif

