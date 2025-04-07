#ifndef VERTEX_HPP
#define VERTEX_HPP

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include <glm/gtx/string_cast.hpp>
#include <iostream>

class Vertex{
public:
    Vertex();
    Vertex(glm::vec3 in_coordinates);
    Vertex(glm::vec3 in_coordinates, glm::vec3 in_color);
    void printVertex();

    glm::vec3 coordinates;
    glm::vec3 color;

private:
    
};

#endif
