#include "Vertex.hpp"

Vertex::Vertex() {}

Vertex::Vertex(glm::vec3 in_coordinates) {
    coordinates = in_coordinates;
}

Vertex::Vertex(glm::vec3 in_coordinates, glm::vec3 in_color) {
    coordinates = in_coordinates;
    color = in_color;
}

Vertex::Vertex(glm::vec3 in_coordinates, glm::vec3 in_color, glm::vec3 in_normals) {
    coordinates = in_coordinates;
    color = in_color;
    normals = in_normals;
}

void Vertex::printVertex(std::string type) {
    std::ofstream outFile("output.txt", std::ios::app);
    outFile << "Printing " << type << ": " << glm::to_string(coordinates) << std::endl;
}