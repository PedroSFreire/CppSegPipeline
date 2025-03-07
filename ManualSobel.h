#ifndef MANSOBEL
#define MANSOBEL
#include <string>
#include "ITKIncludes.h"
class ManualSobel
{
public:
    // Sobel kernels for X, Y, and Z
    const int SobelKernel[3][3][3][3] = {
    {
    {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}},
    {{-2, 0, 2}, {-4, 0, 4}, {-2, 0, 2}},
    {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}}
    },
    {
        {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}},
        {{-2, -4, -2}, {0, 0, 0}, {2, 4, 2}},
        {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}}
    },
    {
        {{-1, -2, -1}, {-2, -4, -2}, {-1, -2, -1}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
        {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}}
    }
    };



    // Generic function to compute Sobel filter in any direction
    int ComputeSobelGradient( int* buffer, int x, int y, int z, int width, int height, int depth, int axis);

    // Wrapper functions for each axis
    int getSobelX( int* buffer, int x, int y, int z, int width, int height, int depth);

    int getSobelY( int* buffer, int x, int y, int z, int width, int height, int depth);

    int getSobelZ( int* buffer, int x, int y, int z, int width, int height, int depth);

};
#endif

