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

    void parseModelData(std::string filepath);
    
    std::vector<int> getModelIndices();
    std::vector<Vertex> getModelVertices();
    std::vector<Vertex> getModelNormals();
    std::unordered_map<int, int> getModelNormalsMap();

private:
    std::ofstream outFile;

    // Model information for particle ("sphere")
    std::vector<Vertex> gModelVertices;
    std::vector<Vertex> gModelNormals;
    std::unordered_map<int, int> gModelNormalsMap;
    std::vector<int> gModelIndices;

};



#endif