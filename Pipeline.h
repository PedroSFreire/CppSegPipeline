#ifndef Pipeline_H
#define Pipeline_H

#include <iostream>
#include <string>
#include <chrono>

#include "ITKIncludes.h"
#include "defines.h"

#include "ITKHandler.h"
#include "ITKHandlerAsync.h"
#include "ITKHandlerMain.h"

#include "CCLAlgoBase.h"
#include "CCLAlgoAir.h"
#include "CCLAlgoFlatLabels.h"
#include "CCLAlgoLiquidPockets.h"
#include "CCLAlgoFinal.h"
#include <future>
#include <thread>


class Pipeline
{
public:
	itk::Size<3> imgSize;
	std::string filename;
	ITKHandlerMain handlerMain;
	ITKHandlerAsync handlerAsync;
	ImageType::Pointer airImg, flatImg, finalImg;
	std::future<void> future;
	CCLAlgoAir* AirCCL;
	CCLAlgoFlatLabels* FlatCCL;
	CCLAlgoLiquidPockets* LiquidCCL;
	CCLAlgoFinal* FinalCCL;


	void runMainPipeline();

	void runFinalPipeline();

	int fullPipeline();
		
};

#endif

