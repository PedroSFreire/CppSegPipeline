#include <iostream>
#include <string>
#include "ITKIncludes.h"
#include <chrono>
#include "ITKHandler.h"

int main() {
    // Ask user for the file name

    std::string filename;
    std::cout << "Enter the NIfTI file path: ";
    std::getline(std::cin, filename);

	ITKHandler handler;
	handler.setupFilters(filename);
	handler.setupPipeline();


    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

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


        

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[µs]" << std::endl;



    return EXIT_SUCCESS;
}