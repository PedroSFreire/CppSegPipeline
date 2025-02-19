#include <iostream>
#include <string>
#include "itkImageFileReader.h"
#include "itkImage.h"
#include "itkImageIOFactory.h"
#include "itkNiftiImageIOFactory.h"
#include <filesystem>
#include <itkBinaryThresholdImageFilter.h>
#include "itkImageFileWriter.h"
#include <itkConnectedComponentImageFilter.h>
#include <itkRelabelComponentImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkLabelGeometryImageFilter.h>



int main() {
    // Ask user for the file name
    std::string filename;
    std::cout << "Enter the NIfTI file path: ";
    std::getline(std::cin, filename);
    itk::NiftiImageIOFactory::RegisterOneFactory();
    // Define the image type (3D float image)
    using ImageType = itk::Image<INT32, 3>;




    // Create the reader
    using ReaderType = itk::ImageFileReader<ImageType>;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(filename);
    ImageType::Pointer image;

     //writer setup
    using WriterType = itk::ImageFileWriter<ImageType>;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName("Seg" + filename);


    //Define threshold filter type
    using ThresholdFilterType = itk::BinaryThresholdImageFilter<ImageType, ImageType>;
    ThresholdFilterType::Pointer thresholdFilter = ThresholdFilterType::New();

	//Threshold filter setup
    thresholdFilter->SetInput(reader->GetOutput());
    thresholdFilter->SetUpperThreshold(-800);  // Pixels below 50 become 0
    thresholdFilter->SetInsideValue(1);
    thresholdFilter->SetOutsideValue(0);

	//Connected component labeling filter
    using ConnectedComponentFilterType = itk::ConnectedComponentImageFilter<ImageType, ImageType>;
    ConnectedComponentFilterType::Pointer ccFilter = ConnectedComponentFilterType::New();

	//Relabeling filter setup
    using RelabelFilterType = itk::RelabelComponentImageFilter<ImageType, ImageType>;
    RelabelFilterType::Pointer relabelFilter = RelabelFilterType::New();
    relabelFilter->SetMinimumObjectSize(100000);


    //Small CC removal filter setup
    using RelabelFilterType = itk::RelabelComponentImageFilter<ImageType, ImageType>;
    RelabelFilterType::Pointer relabelFilter = RelabelFilterType::New();
    relabelFilter->SetMinimumObjectSize(100000);


	// Pipeline setup
    thresholdFilter->SetInput(reader->GetOutput());
    ccFilter->SetInput(thresholdFilter->GetOutput());
    relabelFilter->SetInput(ccFilter->GetOutput());
    writer->SetInput(relabelFilter->GetOutput());



    try {
        // Read the image
        writer->Update();
        image = ccFilter->GetOutput();

        // Print basic information
        ImageType::SizeType size = image->GetLargestPossibleRegion().GetSize();
        ImageType::SpacingType spacing = image->GetSpacing();
        ImageType::PointType origin = image->GetOrigin();

        std::cout << "\nNIfTI Image Loaded Successfully!\n";
        std::cout << "Image Size: " << size[0] << " x " << size[1] << " x " << size[2] << std::endl;
        std::cout << "Voxel Spacing: " << spacing[0] << ", " << spacing[1] << ", " << spacing[2] << std::endl;
        std::cout << "Origin: " << origin[0] << ", " << origin[1] << ", " << origin[2] << std::endl;
    }
    catch (itk::ExceptionObject& err) {
        std::cerr << "Error reading file: " << err << std::endl;
        return EXIT_FAILURE;
    }


    //Start Code





    return EXIT_SUCCESS;
}