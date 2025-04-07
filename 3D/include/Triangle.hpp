#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "glm/glm.hpp"
#include "Vertex.hpp"
#include <vector>
#include <iostream>

class Triangle{
public:

    Triangle();
    Triangle(Vertex v1, Vertex v2, Vertex v3);
    Triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);

    std::vector<Vertex> getVertices();
    void printTriangleInfo();

private:
    Vertex vertex1;
    Vertex vertex2;
    Vertex vertex3;
};




#endif
