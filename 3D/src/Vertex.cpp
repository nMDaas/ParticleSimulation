#include "Vertex.hpp"

Vertex::Vertex() {}

Vertex::Vertex(glm::vec3 in_coordinates) {
    coordinates = in_coordinates;
}

Vertex::Vertex(glm::vec3 in_coordinates, glm::vec3 in_color) {
    coordinates = in_coordinates;
    color = in_color;
}

void Vertex::printVertex() {
    std::cout << "Vertex: " << glm::to_string(coordinates) << std::endl;
}