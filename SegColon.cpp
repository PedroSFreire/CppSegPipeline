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
    //Launch async thread
    std::future<void> future = std::async(std::launch::async, &ITKHandler::setupFilterPipeline,&handler2, filename);
    
	//Init required images to hold each step of the pipeline
    ImageType::Pointer airImg,flatImg,finalImg;

    try {
        // Read the image

		//runs initial air threshold and ccl
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
		//generates auxiliar structures from the labeled air images  and removes small and some selected components
		AirCCL.run();
        

		//generates flat labels from the air image and then runs ccl on it
		handler.runPipelineFlats(airImg);
        flatImg = handler.ccFilter->GetOutput();
        flatImg->DisconnectPipeline();

        //handler.writer->SetInput(airImg);
        //handler.writer->SetFileName("AirLabels" + filename);
        //handler.writer->Update();



		





		

        CCLAlgoFlatLabels flatCCL(flatImg->GetBufferPointer(), handler.getObjCount(), imgSize[0], imgSize[1], imgSize[2],handler.readerOg->GetOutput()->GetBufferPointer());
        //generates auxiliar structures from the labeled flat images  and removes small and some selected components
        flatCCL.run();


        //handler.writer->SetInput(flatImg);
        //handler.writer->SetFileName("FlatLabels" + filename);
        //handler.writer->Update();
		
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::cout << "main stack = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[µs]" << std::endl;

        future.get();


        CCLAlgoLiquidPockets LiquidCCL(handler2.ccFilter->GetOutput()->GetBufferPointer(), handler2.getObjCount(), imgSize[0], imgSize[1], imgSize[2]);
        //generates auxiliar structures from the labeled flat images  and removes small and some selected components
        LiquidCCL.run();

        //handler.writer->SetInput(handler2.ccFilter->GetOutput());
        //handler.writer->SetFileName("LiquidLabels" + filename);
        //handler.writer->Update();

        //intersect the labels from flat and liquid and filters all components 
        LiquidCCL.intersectLabels(flatImg->GetBufferPointer(), airImg->GetBufferPointer(), flatCCL.objCount, AirCCL.objCount, flatCCL.ccVec,airImg->GetLargestPossibleRegion());
        LiquidCCL.addValidCC();
        AirCCL.addValidCC(LiquidCCL.airLabelsValid, LiquidCCL.finalImg->GetBufferPointer());
        flatCCL.addValidCC(LiquidCCL.flatLabelsValid, LiquidCCL.finalImg->GetBufferPointer());


		// does the final expansion of the flat labels aplied to only the valid labels
		flatCCL.labelFinalExpansion(LiquidCCL.flatLabelsValid, LiquidCCL.finalImg->GetBufferPointer(), AirCCL.imageBuffer);
;
        


       // handler.writer->SetInput(flatImg);
        //handler.writer->SetFileName("FlatLabels2" + filename);
        //handler.writer->Update();

		//run ccl in final label
		handler.ccFilter->SetInput(LiquidCCL.finalImg);
		handler.ccFilter->Update();
		finalImg = handler.ccFilter->GetOutput();
        CCLAlgoFinal FinalCCL(finalImg->GetBufferPointer(), handler.getObjCount(), imgSize[0], imgSize[1], imgSize[2]);
        //select on the colon component
		FinalCCL.runCCL();


        //print final label
        handler.writer->SetInput(finalImg);
        handler.writer->SetFileName("FinalLabels" + filename);
        handler.writer->Update();


        end = std::chrono::steady_clock::now();
        std::cout << "full stack = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[µs]" << std::endl;




        



    return EXIT_SUCCESS;
}