#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> 

#include <cstdlib>  // for rand, srand
#include <ctime>    // for time

#ifndef MODEL_PROCESSOR_HPP
#define MODEL_PROCESSOR_HPP

// C++ Standard Template Library (STL)
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>

// Our libraries
#include "Vertex.hpp"
#include "Triangle.hpp"

class ModelProcessor{
public:
    ModelProcessor();

    void VertexSpecification(int gSolverGetParticlesSize);
    void CleanUp(int gSolverGetParticlesSize);
    int getObjTotalIndices(std::string objName);

    std::unordered_map<std::string, std::vector<GLuint>> getGVertexArrayObjects_map();
    std::unordered_map<std::string, std::vector<GLuint>> getGVertexBufferObjects_map();

private:
    std::ofstream outFile;

    // Map that stores bufers for each object in the scene
    std::unordered_map<std::string, std::vector<GLuint>> gVertexArrayObjects_map; 
    std::unordered_map<std::string, std::vector<GLuint>> gVertexBufferObjects_map;
    std::unordered_map<std::string, std::vector<GLuint>> gIndexBufferObjects_map;

    void GenerateGLuintObjects(int gSolverGetParticlesSize, std::string objName);
    void GenerateModelBufferData(int numObjects, std::string particleObjFilepath, std::string objName); // (1)
    void GenerateModelBlueprint(std::string particleObjFilepath, std::string objName); // (2)
    void PrepareAndSendRenderDataToBuffers(int numObjects, std::string objName); // (3)
    void GenerateModelData(std::string modelObjFilepath, std::string objName); // (2) (a)
    void ParseModelData(std::string filepath, std::string objName);
    void getModelMesh(std::string objName);
    std::vector<GLfloat> getVerticesAndAddColorData(std::string objName, glm::vec3 color);
    void offsetGModelIndices(std::string objName); // (2) (b)
    void ConfigureVertexAttributes(std::string objName); // (3) (a)

    std::unordered_map<std::string, std::vector<Vertex>> gModelVertices_map;
    std::unordered_map<std::string, std::vector<Vertex>> gModelNormals_map;
    std::unordered_map<std::string, std::vector<int>> gModelIndices_map;
    std::unordered_map<std::string,  std::unordered_map<int, int>> gModelNormalsMap_map;
    std::unordered_map<std::string,  std::vector<Triangle>> gMesh_map;
    std::unordered_map<std::string,  size_t> gTotalIndices_map;
    std::unordered_map<std::string,  std::string> modelObjFilepath_map;
};



#endif