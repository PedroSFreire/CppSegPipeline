#ifndef GPUHandler_H
#define GPUHandler_H

#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>



class GPUHandler
{
public:

    const char* shaderSource = R"(
#version 430 core
layout (local_size_x = 8, local_size_y = 8, local_size_z = 8) in;
layout (binding = 0, r32f) uniform image3D inputVolume;
uniform float lambda = 0.12;
uniform float k =55;

void main() {
    ivec3 pos = ivec3(gl_GlobalInvocationID);
    ivec3 size = imageSize(inputVolume);
    if (pos.x >= size.x || pos.y >= size.y || pos.z >= size.z) return;

    float I = imageLoad(inputVolume, pos).r;
    float Ix1 = imageLoad(inputVolume, pos + ivec3(1, 0, 0)).r;
    float Ix2 = imageLoad(inputVolume, pos - ivec3(1, 0, 0)).r;
    float Iy1 = imageLoad(inputVolume, pos + ivec3(0, 1, 0)).r;
    float Iy2 = imageLoad(inputVolume, pos - ivec3(0, 1, 0)).r;
    float Iz1 = imageLoad(inputVolume, pos + ivec3(0, 0, 1)).r;
    float Iz2 = imageLoad(inputVolume, pos - ivec3(0, 0, 1)).r;

    float cX1 = exp(-pow(abs(Ix1 - I) / k, 2.0));
    float cX2 = exp(-pow(abs(Ix2 - I) / k, 2.0));
    float cY1 = exp(-pow(abs(Iy1 - I) / k, 2.0));
    float cY2 = exp(-pow(abs(Iy2 - I) / k, 2.0));
    float cZ1 = exp(-pow(abs(Iz1 - I) / k, 2.0));
    float cZ2 = exp(-pow(abs(Iz2 - I) / k, 2.0));

    float diffusion = lambda * (cX1 * (Ix1 - I) + cX2 * (Ix2 - I) +
                                cY1 * (Iy1 - I) + cY2 * (Iy2 - I) +
                                cZ1 * (Iz1 - I) + cZ2 * (Iz2 - I));

    imageStore(inputVolume, pos, vec4(I + diffusion, 0.0, 0.0, 0.0));
}
)";






    GLuint inputTexture;
	GLuint computeShader;
	GLuint shaderProgram;
    GLuint outputTexture;
    int width, height, depth;
    GLuint shader;
    int numIterations = 15;

	GPUHandler(int width, int height, int depth);
    void RunComputeShader( int iterations);

    void InitOpenGL();


    void LoadComputeShader();

    // Load image into OpenGL texture
    void LoadTexture(float* buffer);

    // Save OpenGL texture to file
    void SaveTexture(float* buffer);

    void run(float* buffer);

};


#endif
