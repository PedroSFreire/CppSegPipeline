#include "GPUHandler.h"




GPUHandler::GPUHandler(int width, int height, int depth) {
	this->width = width;
	this->height = height;
	this->depth = depth;
}
void GPUHandler::InitOpenGL() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        exit(EXIT_FAILURE);
    }

    // Create a hidden window (headless mode)
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // This makes the window invisible
    GLFWwindow* window = glfwCreateWindow(1, 1, "Hidden", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glewInit();
}


void GPUHandler::LoadTexture(float* buffer) {
    glGenTextures(1, &inputTexture);
    glBindTexture(GL_TEXTURE_3D, inputTexture);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, width, height, depth, 0, GL_RED, GL_FLOAT, buffer);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);




    glGenTextures(1, &outputTexture);
    glBindTexture(GL_TEXTURE_3D, outputTexture);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, width, height, depth, 0, GL_RED, GL_FLOAT, nullptr);



}


void GPUHandler::LoadComputeShader() {
    shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Compute Shader Compilation Failed:\n" << log << "\n";
        exit(EXIT_FAILURE);
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, shader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, log);
        std::cerr << "Compute Shader Linking Failed:\n" << log << "\n";
        exit(EXIT_FAILURE);
    }

}




void GPUHandler::SaveTexture(float* buffer) {

    glBindTexture(GL_TEXTURE_3D, inputTexture);
    glGetTexImage(GL_TEXTURE_3D, 0, GL_RED, GL_FLOAT, buffer);
    

}





void GPUHandler::RunComputeShader( int iterations) {
    glUseProgram(shaderProgram);

    glBindImageTexture(0, inputTexture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);
    //glBindImageTexture(1, outputTexture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);

    for (int i = 0; i < iterations; i++) {
        glDispatchCompute((width + 7) / 8, (height + 7) / 8, (depth + 7) / 8);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT); // Ensure sync between iterations
    }
}


void GPUHandler::run(float* buffer) {
    InitOpenGL();
    LoadTexture(buffer);
	LoadComputeShader();
	RunComputeShader(numIterations);
	SaveTexture(buffer);
}






