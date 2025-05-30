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
#include "Container.hpp"

#ifndef RENDERER_HPP
#define RENDERER_HPP

class Renderer{
public:
    Renderer();
    Renderer(int i_screenWidth, int i_screenHeight, Scene* scene);

    void updateZ(float val);

    void CreateGraphicsPipelines();
    void RenderScene();
    void CleanUp();

    // All for RayMarching
    GLuint gVertexArrayObject					= 0;
    GLuint gVertexBufferObject					= 0;
    GLuint gIndexBufferObject                  = 0;

    void VertexSpecification(); // for RayMarching
    void PreDraw_RM(); // for RayMarching
    void Draw_RM(); // for RayMarching

private:
    int screenWidth;
    int screenHeight; 
    Scene* mainScene;

    float rotZ; // for container

    GLuint gGraphicsPipelineShaderProgram = 0;
    GLuint gGraphicsLighterPipelineShaderProgram = 0;
    GLuint gGraphicsRayMarchingPipelineShaderProgram = 0;

    std::string LoadShaderAsString(const std::string& filename);
    GLuint CreateShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
    GLuint CompileShader(GLuint type, const std::string& source);

    void PreDraw();
    void DrawParticles(int gTotalIndices);
    void PreDrawParticle(int i);
    void DrawParticle(int i, int gTotalIndices);
    void DrawLights(int gTotalIndices);
    void PreDrawLight();
    void DrawLight(int gTotalIndices);
    void DrawBox(int gBoxTotalIndices);
    void PreDrawBox();
    void DrawBoxActually(int gBoxTotalIndices);
    
};



#endif