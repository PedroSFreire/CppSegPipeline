#ifndef ITKHandlerMAIN_H
#define ITKHandlerMAIN_H
#include "ITKHandler.h"
class ITKHandlerMain : public ITKHandler
{


public:
	ITKHandlerMain();
	void runPipelineFlats(ImageType::Pointer img);
	void runPipelineAir();
	void setupFilters(std::string filename);
};
#endif

