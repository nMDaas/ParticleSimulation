// Third Party Libraries
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

// C++ Standard Template Library (STL)
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>

#include "Solver.hpp"
#include "Particle.hpp"
#include "Camera.hpp"
#include "Vertex.hpp"
#include "Triangle.hpp"
#include "Scene.hpp"

#ifndef RENDERER_HPP
#define RENDERER_HPP

class Renderer{
public:
    Renderer();
    Renderer(int i_screenWidth, int i_screenHeight, Scene* scene);

    void CreateGraphicsPipelines();
    void RenderScene(int gTotalIndices, Camera gCamera);

private:
    int screenWidth;
    int screenHeight; 
    Scene* mainScene;

    GLuint gGraphicsLighterPipelineShaderProgram = 0;

    std::string LoadShaderAsString(const std::string& filename);
    GLuint CreateShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
    GLuint CompileShader(GLuint type, const std::string& source);

    void PreDraw();
    void DrawLights(int gTotalIndices, Camera gCamera);
    void PreDrawLight();
    void DrawLight(int gTotalIndices);
};



#endif