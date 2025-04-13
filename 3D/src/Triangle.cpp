#include "Triangle.hpp"

Triangle::Triangle() {}

Triangle::Triangle(Vertex v1, Vertex v2, Vertex v3) {
    vertex1 = Vertex(v1.coordinates, glm::vec3(0.0f, 0.3f, 0.7f));
    vertex2 = Vertex(v2.coordinates, glm::vec3(0.0f, 0.3f, 0.7f));
    vertex3 = Vertex(v3.coordinates, glm::vec3(0.0f, 0.3f, 0.7f));
}

Triangle::Triangle(Vertex v1, Vertex v2, Vertex v3, Vertex n1, Vertex n2, Vertex n3) {
    vertex1 = Vertex(v1.coordinates, glm::vec3(0.0f, 0.3f, 0.7f), glm::normalize(n1.coordinates));
    vertex2 = Vertex(v2.coordinates, glm::vec3(0.0f, 0.3f, 0.7f), glm::normalize(n2.coordinates));
    vertex3 = Vertex(v3.coordinates, glm::vec3(0.0f, 0.3f, 0.7f), glm::normalize(n3.coordinates));
}

Triangle::Triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
    vertex1 = Vertex(v1, glm::vec3(0.0f, 0.3f, 0.7f));
    vertex2 = Vertex(v2, glm::vec3(0.0f, 0.3f, 0.7f));
    vertex3 = Vertex(v3, glm::vec3(0.0f, 0.3f, 0.7f));
}

std::vector<Vertex> Triangle::getVertices() {
    std::vector<Vertex> result;
    result.push_back(vertex1);
    result.push_back(vertex2);
    result.push_back(vertex3);

    return result;
}

void Triangle::printTriangleInfo() {
    std::cout << "TriangleInfo" << std::endl;
    vertex1.printVertex("Vertex");
    vertex2.printVertex("Vertex");
    vertex3.printVertex("Vertex");
}