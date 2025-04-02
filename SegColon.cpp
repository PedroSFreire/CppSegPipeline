#include <iostream>
#include <string>
#include "ITKIncludes.h"
#include <chrono>
#include "ITKHandler.h"

#include "CCLAlgoBase.h"
#include "CCLAlgoAir.h"
#include "CCLAlgoFlatLabels.h"
#include "CCLAlgoLiquidPockets.h"
#include "CCLAlgoFinal.h"
#include <future>
#include <thread>


int main() {
    // Ask user for the file name
	itk::Size<3> imgSize;
    std::string filename;
    std::cout << "Enter the NIfTI file path: ";
    std::getline(std::cin, filename);
	ITKHandler handler;
    ITKHandler handler2;
    
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	handler.setupFilters(filename);
	handler.setupPipeline();
   
    handler2.setupFilters(filename);
    //handler2.setupFilterPipeline(filename);
    std::future<void> future = std::async(std::launch::async, &ITKHandler::setupFilterPipeline,&handler2, filename);
    

    ImageType::Pointer airImg,flatImg,finalImg;


	int count = 0;
    int num = 0;
    try {
        // Read the image


		handler.runPipelineAir();
        
    }
    catch (itk::ExceptionObject& err) {
        std::cerr << "Error reading file: " << err << std::endl;
        return EXIT_FAILURE;
    }


    //Start Code


        


        
        airImg = handler.ccFilter->GetOutput();
        airImg->DisconnectPipeline();
		imgSize = airImg->GetLargestPossibleRegion().GetSize();
        CCLAlgoAir AirCCL(airImg->GetBufferPointer(),handler.getObjCount(), imgSize[0], imgSize[1], imgSize[2]);
       
		AirCCL.runCCL();
        
		AirCCL.removeUnwantedAirCC(5000);

		handler.runPipelineFlats(airImg);

        handler.writer->SetInput(airImg);
        handler.writer->SetFileName("AirLabels" + filename);
        handler.writer->Update();



		flatImg = handler.ccFilter->GetOutput();





		flatImg->DisconnectPipeline();

        CCLAlgoFlatLabels flatCCL(flatImg->GetBufferPointer(), handler.getObjCount(), imgSize[0], imgSize[1], imgSize[2],handler.readerOg->GetOutput()->GetBufferPointer());

        flatCCL.runCCL();
        flatCCL.removeSmallCC(300);

        handler.writer->SetInput(flatImg);
        handler.writer->SetFileName("FlatLabels" + filename);
        handler.writer->Update();
		
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::cout << "main stack = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[µs]" << std::endl;

        future.get();


        CCLAlgoLiquidPockets LiquidCCL(handler2.ccFilter->GetOutput()->GetBufferPointer(), handler2.getObjCount(), imgSize[0], imgSize[1], imgSize[2]);
        LiquidCCL.runCCL();
        LiquidCCL.removeSmallCC(5000);
        LiquidCCL.removeBigCC(0.02 * imgSize[0]* imgSize[1]* imgSize[2]);


        handler.writer->SetInput(handler2.ccFilter->GetOutput());
        handler.writer->SetFileName("LiquidLabels" + filename);
        handler.writer->Update();


        LiquidCCL.intersectLabels(flatImg->GetBufferPointer(), airImg->GetBufferPointer(), flatCCL.objCount, AirCCL.objCount, flatCCL.ccVec,airImg->GetLargestPossibleRegion());
        LiquidCCL.addValidCC();
        AirCCL.addValidCC(LiquidCCL.airLabelsValid, LiquidCCL.finalImg->GetBufferPointer());
        flatCCL.addValidCC(LiquidCCL.flatLabelsValid, LiquidCCL.finalImg->GetBufferPointer());

		flatCCL.labelFinalExpansion(LiquidCCL.flatLabelsValid, LiquidCCL.finalImg->GetBufferPointer(), AirCCL.imageBuffer);
        //LiquidCCL.liquidFinalExpansion(handler.reader->GetOutput()->GetBufferPointer());
        


        handler.writer->SetInput(flatImg);
        handler.writer->SetFileName("FlatLabels2" + filename);
        handler.writer->Update();


		handler.ccFilter->SetInput(LiquidCCL.finalImg);
		handler.ccFilter->Update();
		finalImg = handler.ccFilter->GetOutput();
        CCLAlgoFinal FinalCCL(finalImg->GetBufferPointer(), handler.getObjCount(), imgSize[0], imgSize[1], imgSize[2]);
		FinalCCL.runCCL();

        handler.writer->SetInput(finalImg);
        handler.writer->SetFileName("FinalLabels" + filename);
        handler.writer->Update();


        end = std::chrono::steady_clock::now();
        std::cout << "full stack = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[µs]" << std::endl;




        



    return EXIT_SUCCESS;
}