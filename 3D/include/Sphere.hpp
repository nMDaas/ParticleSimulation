#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> 

#ifndef SPHERE_HPP
#define SPHERE_HPP

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

class Sphere{
public:
    Sphere();

    void VertexSpecification(int gSolverGetParticlesSize);
    void CleanUp(int gSolverGetParticlesSize);
    int getTotalIndices();
    int getBoxTotalIndices();

    std::vector<int> getModelIndices();
    std::vector<Vertex> getModelVertices();
    std::vector<Vertex> getModelNormals();
    std::unordered_map<int, int> getModelNormalsMap();

    std::vector<GLuint> getGVertexArrayObjects();
    std::vector<GLuint> getGVertexBufferObjects();
    GLuint* getLightVertexArrayObject();
    GLuint* getLightVertexBufferObject();
    GLuint* getBoxVertexArrayObject();
    GLuint* getBoxVertexBufferObject();

private:
    std::ofstream outFile;

    // Model information for particle ("sphere")
    /*std::vector<Vertex> gModelVertices;
    std::vector<Vertex> gModelNormals;
    std::unordered_map<int, int> gModelNormalsMap;
    std::vector<int> gModelIndices;*/

    // Model information for box
    std::vector<Vertex> gModelBoxVertices;
    std::vector<Vertex> gModelBoxNormals;
    std::vector<int> gModelBoxIndices;
    std::unordered_map<int, int> gModelBoxNormalsMap;
    std::vector<Triangle> gBoxMesh;
    size_t gTotalBoxIndices = 0;

    std::vector<std::vector<GLfloat>> gVertexData;
    size_t gTotalIndices = 0;

    // Particle information
    std::vector<GLuint> gVertexArrayObjects;
    std::vector<GLuint> gVertexBufferObjects;
    std::vector<GLuint> gIndexBufferObjects;

    // Light information
    GLuint lightVertexArrayObject = 0;
    GLuint lightVertexBufferObject = 0;
    GLuint lightIndexBufferObject = 0;

    // Box information
    GLuint boxVertexArrayObject = 0;
    GLuint boxVertexBufferObject = 0;
    GLuint boxIndexBufferObject = 0;

    void GenerateGLuintObjects(int gSolverGetParticlesSize);
    void GenerateGLuintLight();
    void GenerateGluintBoxObjects();
    void GenerateModelBufferData(int gSolverGetParticlesSize);
    void GenerateModelData(std::string modelObjFilepath, std::string objName);
    void GenerateParticleModelData();
    void ParseModelData(std::string filepath, std::string objName);
    //void ParseModelDataOld(std::string filepath);
    void getModelMesh(std::string objName);
    //void getModelMeshOld();
    std::vector<GLfloat> getVerticesAndAddColorData();
    //void offsetGModelIndicesOld();
    void offsetGModelIndices(std::string objName);
    void ConfigureVertexAttributes();
    void GenerateLightBufferData();
    void ConfigureLightVertexAttributes();
    void GenerateBoxBufferData();
    void GenerateBoxModelData();
    void offsetGBoxIndices();
    void ParseBoxModelData(std::string filepath);
    void getBoxModelMesh();
    std::vector<GLfloat> getBoxVerticesAndAddColorData();
    void ConfigureBoxVertexAttributes();

    std::unordered_map<std::string, std::vector<Vertex>> gModelVertices_map;
    std::unordered_map<std::string, std::vector<Vertex>> gModelNormals_map;
    std::unordered_map<std::string, std::vector<int>> gModelIndices_map;
    std::unordered_map<std::string,  std::unordered_map<int, int>> gModelNormalsMap_map;
    std::unordered_map<std::string,  std::vector<Triangle>> gMesh_map;
};



#endif