#ifndef ITKIncludes_H
#define ITKIncludes_H

#include "itkImageFileReader.h"
#include "itkImage.h"
#include "itkMultiThreaderBase.h"
#include "itkImageIOFactory.h"
#include "itkNiftiImageIOFactory.h"
#include <filesystem>
#include <itkBinaryThresholdImageFilter.h>
#include "itkImageFileWriter.h"
#include <itkConnectedComponentImageFilter.h>
#include <itkRelabelComponentImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkLabelImageToShapeLabelMapFilter.h>
#include <itkLabelStatisticsImageFilter.h>

#include "itkNeighborhoodOperatorImageFilter.h"
#include "itkSobelOperator.h"
#include "itkClampImageFilter.h"
#include "itkHistogramMatchingImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"








#endif