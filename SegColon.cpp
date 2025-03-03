#include <iostream>
#include <string>
#include "ITKIncludes.h"
#include <chrono>
#include "ITKHandler.h"
#include "CCLAlgo.h"

int main() {
    // Ask user for the file name
	itk::Size<3> imgSize;
    std::string filename;
    std::cout << "Enter the NIfTI file path: ";
    std::getline(std::cin, filename);
	ITKHandler handler;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	handler.setupFilters(filename);
	handler.setupPipeline();


    

    ImageType::Pointer airImg,flatImg;


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
        CCLAlgo AirCCL(airImg->GetBufferPointer(),handler.getObjCount(), imgSize[0], imgSize[1], imgSize[2]);
       
		AirCCL.runCCL();
        
		AirCCL.removeUnwantedAirCC(5000);

		handler.runPipelineFlats(airImg);

		flatImg = handler.ccFilter->GetOutput();
		flatImg->DisconnectPipeline();
        CCLAlgo flatCCL(flatImg->GetBufferPointer(), handler.getObjCount(), imgSize[0], imgSize[1], imgSize[2]);

        flatCCL.runCCL();

        flatCCL.removeSmallCC(60);


        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::cout << "before writing = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[µs]" << std::endl;
		handler.writer->SetInput(airImg);
		handler.writer->SetFileName("Air.nii");
		handler.writer->Update();

        handler.writer->SetInput(flatImg);
		handler.writer->SetFileName("Sobel.nii");
		handler.writer->Update();



        




        



    return EXIT_SUCCESS;
}