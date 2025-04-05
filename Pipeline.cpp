#include "Pipeline.h"





void Pipeline::runMainPipeline() {
    handlerMain.runPipelineAir();

    airImg = handlerMain.ccFilter->GetOutput();
    airImg->DisconnectPipeline();
    imgSize = airImg->GetLargestPossibleRegion().GetSize();

    AirCCL = new CCLAlgoAir(airImg->GetBufferPointer(), handlerMain.getObjCount(), imgSize[0], imgSize[1], imgSize[2]);
    //generates auxiliar structures from the labeled air images  and removes small and some selected components
    AirCCL->run();


    //generates flat labels from the air image and then runs ccl on it
    handlerMain.runPipelineFlats(airImg);
    flatImg = handlerMain.ccFilter->GetOutput();
    flatImg->DisconnectPipeline();

    if (DEBUG_AIR) {
        handlerMain.writer->SetInput(airImg);
        handlerMain.writer->SetFileName("AirLabels" + filename);
        handlerMain.writer->Update();
    }

    FlatCCL = new CCLAlgoFlatLabels(flatImg->GetBufferPointer(), handlerMain.getObjCount(), imgSize[0], imgSize[1], imgSize[2], handlerMain.readerOg->GetOutput()->GetBufferPointer());
    //generates auxiliar structures from the labeled flat images  and removes small and some selected components
    FlatCCL->run();

    if (DEBUG_FLAT) {
        handlerMain.writer->SetInput(flatImg);
        handlerMain.writer->SetFileName("FlatLabels" + filename);
        handlerMain.writer->Update();
    }

}



void Pipeline::runFinalPipeline() {
    LiquidCCL = new CCLAlgoLiquidPockets(handlerAsync.ccFilter->GetOutput()->GetBufferPointer(), handlerAsync.getObjCount(), imgSize[0], imgSize[1], imgSize[2]);
    //generates auxiliar structures from the labeled flat images  and removes small and some selected components
    LiquidCCL->run();


    if (DEBUG_LIQUID) {
        handlerAsync.writer->SetInput(handlerAsync.ccFilter->GetOutput());
        handlerAsync.writer->SetFileName("LiquidLabels" + filename);
        handlerAsync.writer->Update();
    }


    //intersect the labels from flat and liquid and filters all components 
    LiquidCCL->intersectLabels(flatImg->GetBufferPointer(), airImg->GetBufferPointer(), FlatCCL->objCount, AirCCL->objCount, FlatCCL->ccVec, airImg->GetLargestPossibleRegion());
    LiquidCCL->addValidCC();
    AirCCL->addValidCC(LiquidCCL->airLabelsValid, LiquidCCL->finalImg->GetBufferPointer());
    FlatCCL->addValidCC(LiquidCCL->flatLabelsValid, LiquidCCL->finalImg->GetBufferPointer());


    // does the final expansion of the flat labels aplied to only the valid labels
    FlatCCL->labelFinalExpansion(LiquidCCL->flatLabelsValid, LiquidCCL->finalImg->GetBufferPointer(), AirCCL->imageBuffer);
    

     //run ccl in final label
    handlerMain.ccFilter->SetInput(LiquidCCL->finalImg);
    handlerMain.ccFilter->Update();
    finalImg = handlerMain.ccFilter->GetOutput();
    FinalCCL = new CCLAlgoFinal(finalImg->GetBufferPointer(), handlerMain.getObjCount(), imgSize[0], imgSize[1], imgSize[2]);
    //select on the colon component
    FinalCCL->runCCL();


    //print final label
    handlerMain.writer->SetInput(finalImg);
    handlerMain.writer->SetFileName("FinalLabels" + filename);
    handlerMain.writer->Update();
}



int Pipeline::fullPipeline() {
    std::cout << "Enter the NIfTI file path: ";
    std::getline(std::cin, filename);
    handlerMain.setupFilters(filename);
    handlerMain.setupPipeline();
    handlerAsync.setupFilters(filename);

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();


    
    //Launch async thread
    future = std::async(std::launch::async, &ITKHandlerAsync::setupFilterPipeline, &handlerAsync, filename);
    
    //Time main Thread
    runMainPipeline();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "main stack = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[µs]" << std::endl;

	//retrieve async thread
    future.get();


    //Time final Thread
    std::chrono::steady_clock::time_point begin2 = std::chrono::steady_clock::now();
    runFinalPipeline();

    //print Times
    end = std::chrono::steady_clock::now();
    std::cout << "Final Syncronous stack = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin2).count() << "[µs]" << std::endl;
    std::cout << "full stack = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[µs]" << std::endl;
    return 1;
}