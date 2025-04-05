#ifndef defines_H
#define defines_H
#include "ITKIncludes.h"

using ImageType = itk::Image<int, 3>;
using ImageFloatType = itk::Image<float, 3>;


//************************************************************************************************************************
//Threshhold values
const int AirThreshold = -600;
const int LiquidThreshold = 210;



//************************************************************************************************************************
//Min size values
//in % of volume
const int AirSizeMin = 0.0001;
const int LiquidSizeMin = 0.0001;

//in number of voxels
const int FlatSizeMin = 300;



//************************************************************************************************************************
//Max forced expansion values
const int LabelExpandTop = 8;
const int LabelExpandBottom = 8;



//************************************************************************************************************************
//Debug Printing
const bool DEBUG_AIR = false;
const bool DEBUG_FLAT = false;
const bool DEBUG_LIQUID = false;


#endif