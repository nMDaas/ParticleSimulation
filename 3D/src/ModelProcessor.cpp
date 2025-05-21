#include "ModelProcessor.hpp"

ModelProcessor::ModelProcessor() : outFile("output.txt"){
    gVertexArrayObjects_map["Particle"] = {};
    gVertexBufferObjects_map["Particle"] = {};
    gIndexBufferObjects_map["Particle"] = {};
    gVertexArrayObjects_map["Light"] = {};
    gVertexBufferObjects_map["Light"] = {};
    gIndexBufferObjects_map["Light"] = {};
    gVertexArrayObjects_map["Box"] = {};
    gVertexBufferObjects_map["Box"] = {};
    gIndexBufferObjects_map["Box"] = {};
    srand(static_cast<unsigned>(time(0)));
}

void ModelProcessor::VertexSpecification(int gSolverGetParticlesSize){
    GenerateGLuintObjects(gSolverGetParticlesSize, "Particle");

    GenerateModelBufferData(gSolverGetParticlesSize, "/Users/natashadaas/ParticleSimulation/3D/src/models/sphereCorrect.obj", "Particle");

    GenerateGLuintObjects(1, "Light");

    GenerateModelBufferData(1, "/Users/natashadaas/ParticleSimulation/3D/src/models/sphereCorrect.obj", "Light");

    GenerateGLuintObjects(1, "Box");

    GenerateModelBufferData(1, "/Users/natashadaas/ParticleSimulation/3D/src/models/cube.obj", "Box");
}

// Generate newGVertexArrayObject, newGVertexBufferObject and newGIndexBufferObject for each particle
void ModelProcessor::GenerateGLuintObjects(int gSolverGetParticlesSize, std::string objName){
    // For Particles
    for (int i = 0; i < gSolverGetParticlesSize; i++) {
        GLuint newGVertexArrayObject = 0;
        GLuint newGVertexBufferObject = 0;
        GLuint newGIndexBufferObject = 0;

        glGenVertexArrays(1, &newGVertexArrayObject);
        glGenBuffers(1, &newGVertexBufferObject);
        glGenBuffers(1, &newGIndexBufferObject);

        // Save after creation
        gVertexArrayObjects_map[objName].push_back(newGVertexArrayObject);
        gVertexBufferObjects_map[objName].push_back(newGVertexBufferObject);
        gIndexBufferObjects_map[objName].push_back(newGIndexBufferObject);

        // Now bind them correctly
        glBindVertexArray(newGVertexArrayObject);
        glBindBuffer(GL_ARRAY_BUFFER, newGVertexBufferObject);
    }
}

// numObjects refers to how many of this specific object
void ModelProcessor::GenerateModelBufferData(int numObjects, std::string particleObjFilepath, std::string objName){
    GenerateModelBlueprint(particleObjFilepath, objName);

    PrepareAndSendRenderDataToBuffers(numObjects, objName);
}

void ModelProcessor::GenerateModelBlueprint(std::string particleObjFilepath, std::string objName){
    for (const auto& pair : modelObjFilepath_map) {
        std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
    }
    if (modelObjFilepath_map.find(objName) == modelObjFilepath_map.end() || modelObjFilepath_map.size() == 0) {
        GenerateModelData(particleObjFilepath, objName); // This creates a particle "blueprint"

        // Fix indices information from 0 - n to 1 - n
        offsetGModelIndices(objName);

        modelObjFilepath_map[objName] = particleObjFilepath;

        std::cout << "Blueprint for " << objName << " created." << std::endl;
    }
    else{
        std::cout << "Blueprint for " << objName << " already created. Skipping." << std::endl;
    }

}

void ModelProcessor::PrepareAndSendRenderDataToBuffers(int numObjects, std::string objName){
    std::vector<std::vector<GLfloat>> gVertexData;

    // Hardcoding colors for particles
    std::vector<glm::vec3> colors = {
        glm::vec3(1.0f, 0.0f, 0.0f), // red
        glm::vec3(0.0f, 1.0f, 0.0f), // green
        glm::vec3(0.0f, 0.0f, 1.0f), // blue
        glm::vec3(1.0f, 0.0f, 1.0f), // magenta
        glm::vec3(1.0f, 1.0f, 0.0f) // yellow?
    };

    // Create vertex data lists for each object instance
    for (int i = 0; i < numObjects; i++) {
        //std::vector<GLfloat> vertexData_i = getVerticesAndAddColorData();
        float x, y, z;
        if (objName == "Particle") {
            x = colors[i].x;
            y = colors[i].y;
            z = colors[i].z;
        }
        else {
            x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        }
        glm::vec3 randColor = glm::vec3(x,y,z);

        std::vector<GLfloat> vertexData_i = getVerticesAndAddColorData(objName, randColor);
        gVertexData.push_back(vertexData_i);
    }

    std::vector<int> gModelIndices = gModelIndices_map[objName];
    int gTotalIndices = gModelIndices.size();
    gTotalIndices_map[objName] = gTotalIndices;

    // Send rendering data to buffers for each object instance
    for (int i = 0; i < numObjects; i++) {

        glGenVertexArrays(1, &gVertexArrayObjects_map[objName][i]);
        glBindVertexArray(gVertexArrayObjects_map[objName][i]);

        glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObjects_map[objName][i]);

        glBufferData(GL_ARRAY_BUFFER, 								// Kind of buffer we are working with 
                                                                    // (e.g. GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER)
                    gVertexData[i].size() * sizeof(GL_FLOAT), 	// Size of data in bytes
                    gVertexData[i].data(), 						// Raw array of data
                    GL_STATIC_DRAW);								// How we intend to use the data

        // Generate EBO
        glGenBuffers(1, &gIndexBufferObjects_map[objName][i]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferObjects_map[objName][i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, gModelIndices.size() * sizeof(GLuint), gModelIndices.data(), GL_STATIC_DRAW);

        ConfigureVertexAttributes(objName);
    }
}

void ModelProcessor::GenerateModelData(std::string modelObjFilepath, std::string objName){
    ParseModelData(modelObjFilepath, objName);
    getModelMesh(objName);
}

void ModelProcessor::ParseModelData(std::string filepath, std::string objName){
    outFile << "--- In parseModelData() ---" << std::endl;
    std::ifstream inputFile(filepath);

    // Check if the file opened successfully
    if (!inputFile) {
        std::cerr << "Error opening file: " << filepath << std::endl;
    }

    std::vector<Vertex> gModelVertices;
    std::vector<Vertex> gModelNormals;
    std::unordered_map<int, int> gModelNormalsMap;
    std::vector<int> gModelIndices;

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

    // Store data in respective maps
    gModelVertices_map[objName] = gModelVertices;
    gModelNormals_map[objName] = gModelNormals;
    gModelIndices_map[objName] = gModelIndices;
    gModelNormalsMap_map[objName] = gModelNormalsMap;

    // Close the file
    inputFile.close();
    outFile << "--- Exiting parseModelData() ---" << std::endl;
}

void ModelProcessor::getModelMesh(std::string objName) {
    outFile << "--- In getModelMesh() ---" << std::endl;

    outFile << "-------" << std::endl;
    outFile << "Collecting Indices into Triangles" << std::endl;
    outFile << "-------" << std::endl;

    std::vector<Vertex> gModelVertices = gModelVertices_map[objName];
    std::vector<Vertex> gModelNormals = gModelNormals_map[objName];
    std::unordered_map<int, int> gModelNormalsMap = gModelNormalsMap_map[objName];
    std::vector<int> gModelIndices = gModelIndices_map[objName];

    std::vector<Triangle> gMesh;

    for (int i = 0; i < gModelIndices.size(); i++) {
        
        outFile << "Triangle" << std::endl;

        outFile << "  Index 1: " << gModelIndices[i] << std::endl;
        outFile << "  Index 2: " << gModelIndices[i+1] << std::endl;
        outFile << "  Index 3: " << gModelIndices[i+2] << std::endl;
        outFile << "  Normal 1: " << gModelNormalsMap[gModelIndices[i] - 1] + 1<< std::endl;
        outFile << "  Normal 2: " << gModelNormalsMap[gModelIndices[i+1] - 1] + 1<< std::endl;
        outFile << "  Normal 3: " << gModelNormalsMap[gModelIndices[i+2] - 1] + 1<< std::endl;

        // print Vertices of Triangle
        outFile << "  Vertices: " << std::endl;
        gModelVertices[gModelIndices[i] - 1].printVertex("Vertex");
        gModelVertices[gModelIndices[i + 1] - 1].printVertex("Vertex");
        gModelVertices[gModelIndices[i + 2] - 1].printVertex("Vertex");

        // print Normals of Triangle
        outFile << "  Normals: " << std::endl;
        gModelVertices[gModelIndices[i + 3] - 1].printVertex("Normal");
        gModelVertices[gModelIndices[i + 4] - 1].printVertex("Normal");
        gModelVertices[gModelIndices[i + 5] - 1].printVertex("Normal");

        Triangle t(gModelVertices[gModelIndices[i] - 1], gModelVertices[gModelIndices[i + 1] - 1], gModelVertices[gModelIndices[i + 2] - 1], gModelVertices[gModelIndices[i + 3] - 1], gModelVertices[gModelIndices[i + 4] - 1], gModelVertices[gModelIndices[i + 5] - 1]);
        i = i + 5;
        gMesh.push_back(t);
    }

    gMesh_map[objName] = gMesh;

    outFile << "-------" << std::endl;
    outFile<< "End Collecting Indices into Triangles" << std::endl;
    outFile << "-------" << std::endl;

   outFile << "--- Exiting getModelMesh() ---" << std::endl;
}

std::vector<GLfloat> ModelProcessor::getVerticesAndAddColorData(std::string objName, glm::vec3 color) {
    std::vector<GLfloat> vertexPositionsAndColor;

    std::vector<Vertex> gModelVertices = gModelVertices_map[objName];
    std::vector<Vertex> gModelNormals = gModelNormals_map[objName];
    std::unordered_map<int, int> gModelNormalsMap = gModelNormalsMap_map[objName];

    for (int i = 0; i < gModelVertices.size(); i++) {
        vertexPositionsAndColor.push_back(gModelVertices[i].coordinates.x);
        vertexPositionsAndColor.push_back(gModelVertices[i].coordinates.y);
        vertexPositionsAndColor.push_back(gModelVertices[i].coordinates.z);
        outFile << "Index: " << i << std::endl;
        outFile << "Vertex: (" << gModelVertices[i].coordinates.x << "," << gModelVertices[i].coordinates.y << "," << gModelVertices[i].coordinates.z << ")" << std::endl;
        vertexPositionsAndColor.push_back(color.x);
        vertexPositionsAndColor.push_back(color.y);
        vertexPositionsAndColor.push_back(color.z);
        vertexPositionsAndColor.push_back(gModelNormals[gModelNormalsMap[i]].coordinates.x);
        vertexPositionsAndColor.push_back(gModelNormals[gModelNormalsMap[i]].coordinates.y);
        vertexPositionsAndColor.push_back(gModelNormals[gModelNormalsMap[i]].coordinates.z);
        outFile << "gModelNormalsMap[i]: " << gModelNormalsMap[i] << std::endl;
        outFile << "Normal: (" << gModelNormals[gModelNormalsMap[i]].coordinates.x << "," << gModelNormals[gModelNormalsMap[i]].coordinates.y << "," << gModelNormals[gModelNormalsMap[i]].coordinates.z << ")" << std::endl;
        
    }

    return vertexPositionsAndColor;
}

void ModelProcessor::offsetGModelIndices(std::string objName) {
   for (int i = 0; i < gModelIndices_map[objName].size(); i++) {
        gModelIndices_map[objName][i] = gModelIndices_map[objName][i] - 1; 
    } 
}

void ModelProcessor::ConfigureVertexAttributes(std::string objName) {
   // Enable the vertex attribute for position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 9, (GLvoid*)0);

    if (objName != "Light" && objName != "Box") {
        // Enable the vertex attribute for color
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 9, (GLvoid*)(sizeof(GL_FLOAT) * 3));

        // Enable the vertex attribute for normal
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 9, (GLvoid*)(sizeof(GL_FLOAT) * 6));
    }

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);

    if (objName != "Light" && objName != "Box") {
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
    }
}

void ModelProcessor::CleanUp(int gSolverGetParticlesSize){
    for (int i = 0; i < gSolverGetParticlesSize; i++){
        glDeleteBuffers(1, &gVertexBufferObjects_map["Particle"][i]);
        glDeleteVertexArrays(1, &gVertexArrayObjects_map["Particle"][i]);
    }

    glDeleteBuffers(1, &gVertexBufferObjects_map["Light"][0]);
    glDeleteVertexArrays(1, &gVertexArrayObjects_map["Light"][0]);

    glDeleteBuffers(1, &gVertexBufferObjects_map["Box"][0]);
    glDeleteVertexArrays(1, &gVertexArrayObjects_map["Box"][0]);
}

int ModelProcessor::getObjTotalIndices(std::string objName){
    return gTotalIndices_map[objName];
}

std::unordered_map<std::string, std::vector<GLuint>> ModelProcessor::getGVertexArrayObjects_map(){
    return gVertexArrayObjects_map;
}

std::unordered_map<std::string, std::vector<GLuint>> ModelProcessor::getGVertexBufferObjects_map(){
    return gVertexBufferObjects_map;
}