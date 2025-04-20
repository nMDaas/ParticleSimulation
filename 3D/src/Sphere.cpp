#include "Sphere.hpp"

Sphere::Sphere() : outFile("output.txt"){}

void Sphere::parseModelData(std::string filepath){
    outFile << "--- In parseModelData() ---" << std::endl;
    std::ifstream inputFile(filepath);

    // Check if the file opened successfully
    if (!inputFile) {
        std::cerr << "Error opening file: " << filepath << std::endl;
    }

    std::string line;
    // Read each line from the file
    while (std::getline(inputFile, line)) {
        std::istringstream stream(line);
        std::string word;
        std::vector<std::string> words;

        while (stream >> word) {
            words.push_back(word); // Add each word to the vector
        }

        if (words[0] == "v") {
            Vertex v(glm::vec3(std::stof(words[1]), std::stof(words[2]), std::stof(words[3])));
            v.printVertex("Vertex");
            gModelVertices.push_back(v);
        }
        if (words[0] == "vn") {
            Vertex v(glm::vec3(std::stof(words[1]), std::stof(words[2]), std::stof(words[3])));
            v.printVertex("Normal");
            gModelNormals.push_back(v);
        }
        if (words[0] == "f") {
            std::string vertexNormalIndex;
            std::vector<int> indices; // for vertices
            std::vector<int> normals; // for normals

            for (int i = 1; i <=3; i++) {
                std::stringstream ss(words[i]);
                ss >> vertexNormalIndex; // vertexNormalIndex refers to vertexIndex//normalIndex
                size_t pos = vertexNormalIndex.find("//");
                std::string vertexIndexPart = vertexNormalIndex.substr(0, pos);
                std::string normalIndexPart = vertexNormalIndex.substr(pos + 2);
                indices.push_back(std::stoi(vertexIndexPart));
                normals.push_back(std::stoi(normalIndexPart));
            }

            outFile << "Indices: " << indices[0] << "," << indices[1] << "," << indices[2] << std::endl;
            outFile << "Normals: " << normals[0] << "," << normals[1] << "," << normals[2] << std::endl;
            outFile << "------" << std::endl;
            gModelIndices.push_back(indices[0]);
            gModelIndices.push_back(indices[1]);
            gModelIndices.push_back(indices[2]);
            gModelIndices.push_back(normals[0]);
            gModelIndices.push_back(normals[1]);
            gModelIndices.push_back(normals[2]);
            gModelNormalsMap[indices[0] - 1] = normals[0] - 1;
            gModelNormalsMap[indices[1] - 1] = normals[1] - 1;
            gModelNormalsMap[indices[2] - 1] = normals[2] - 1;
        }
    }

    // Close the file
    inputFile.close();
    outFile << "--- Exiting parseModelData() ---" << std::endl;
}

std::vector<int> Sphere::getModelIndices(){
    return gModelIndices;
}

std::vector<Vertex> Sphere::getModelVertices(){
    return gModelVertices;
}

std::unordered_map<int, int> Sphere::getModelNormalsMap(){
    return gModelNormalsMap;
}

std::vector<Vertex> Sphere::getModelNormals(){
    return gModelNormals;
}