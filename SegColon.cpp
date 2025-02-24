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
	handler.setupFilters(filename);
	handler.setupPipeline();


    

    ImageType::Pointer image;


	int count = 0;
    int num = 0;

    try {
        // Read the image
		handler.runPipeline();
    }
    catch (itk::ExceptionObject& err) {
        std::cerr << "Error reading file: " << err << std::endl;
        return EXIT_FAILURE;
    }



    //Start Code


        


        
		image = handler.thresholdFilter->GetOutput();
		imgSize = image->GetLargestPossibleRegion().GetSize();
        CCLAlgo AirCCL(image->GetBufferPointer(), imgSize[0], imgSize[1], imgSize[2]);
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
		AirCCL.runCCL();

		/*handler.ccFilter->SetInput(handler.thresholdFilter->GetOutput());
		handler.ccFilter->Update();*/

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();



        std::cout << "CCL run time = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[µs]" << std::endl;



    return EXIT_SUCCESS;
}