#include "ManualSobel.h"




int ManualSobel::ComputeSobelGradient( int* buffer, int x, int y, int z, int width, int height, int depth, int axis) {
    int gradient = 0;

    for (int dz = -1; dz <= 1; dz++) {
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                int nx = x + dx;
                int ny = y + dy;
                int nz = z + dz;

                // Ensure we don't go out of bounds
                if (nx >= 0 && ny >= 0 && nz >= 0 && nx < width && ny < height && nz < depth) {
                    int index = nz * width * height + ny * width + nx;
                    gradient += buffer[index] * SobelKernel[axis][dz + 1][dy + 1][dx + 1];
                }
            }
        }
    }
    return gradient;
}

int ManualSobel::getSobelX( int* buffer, int x, int y, int z, int width, int height, int depth) {
    return ComputeSobelGradient(buffer, x, y, z, width, height, depth, 0);
}

int ManualSobel::getSobelY( int* buffer, int x, int y, int z, int width, int height, int depth) {
    return ComputeSobelGradient(buffer, x, y, z, width, height, depth, 1);
}

int ManualSobel::getSobelZ( int* buffer, int x, int y, int z, int width, int height, int depth) {
    return ComputeSobelGradient(buffer, x, y, z, width, height, depth, 2);
}
