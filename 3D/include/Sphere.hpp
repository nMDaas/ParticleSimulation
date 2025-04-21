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

    std::vector<int> getModelIndices();
    std::vector<Vertex> getModelVertices();
    std::vector<Vertex> getModelNormals();
    std::unordered_map<int, int> getModelNormalsMap();

    std::vector<GLuint> getGVertexArrayObjects();
    std::vector<GLuint> getGVertexBufferObjects();
    GLuint* getLightVertexArrayObject();
    GLuint* getLightVertexBufferObject();

private:
    std::ofstream outFile;

    // Model information for particle ("sphere")
    std::vector<Vertex> gModelVertices;
    std::vector<Vertex> gModelNormals;
    std::unordered_map<int, int> gModelNormalsMap;
    std::vector<int> gModelIndices;

    std::vector<std::vector<GLfloat>> gVertexData;
    std::vector<Triangle> gMesh;
    size_t gTotalIndices = 0;

    // Particle information
    std::vector<GLuint> gVertexArrayObjects;
    std::vector<GLuint> gVertexBufferObjects;
    std::vector<GLuint> gIndexBufferObjects;

    // Light information
    GLuint lightVertexArrayObject = 0;
    GLuint lightVertexBufferObject = 0;
    GLuint lightIndexBufferObject = 0;

    void GenerateGLuintObjects(int gSolverGetParticlesSize);
    void GenerateGLuintLight();
    void GenerateModelBufferData(int gSolverGetParticlesSize);
    void GenerateParticleModelData();
    void ParseModelData(std::string filepath);
    void getModelMesh();
    std::vector<GLfloat> getVerticesAndAddColorData();
    void offsetGModelIndices();
    void ConfigureVertexAttributes();
    void GenerateLightBufferData();
    void ConfigureLightVertexAttributes();

};



#endif