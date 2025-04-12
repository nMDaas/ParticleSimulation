#ifndef VERTEX_HPP
#define VERTEX_HPP

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include <glm/gtx/string_cast.hpp>

// C++ Standard Template Library (STL)
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

class Vertex{
public:
    Vertex();
    Vertex(glm::vec3 in_coordinates);
    Vertex(glm::vec3 in_coordinates, glm::vec3 in_color);
    Vertex(glm::vec3 in_coordinates, glm::vec3 in_color, glm::vec3 in_normals);
    void printVertex(std::string type);

    glm::vec3 coordinates;
    glm::vec3 color;
    glm::vec3 normals;
    static std::ofstream outFile2;

private:
    
};

#endif
