#include "Sphere.hpp"

Sphere::Sphere() : outFile("output.txt"){}

void Sphere::VertexSpecification(int gSolverGetParticlesSize){
    GenerateGLuintObjects(gSolverGetParticlesSize);

    GenerateGLuintLight();

    GenerateGluintBoxObjects();

    GenerateModelBufferData(gSolverGetParticlesSize);

    GenerateLightBufferData();

    GenerateBoxBufferData();
}

// Generate newGVertexArrayObject, newGVertexBufferObject and newGIndexBufferObject for each particle
void Sphere::GenerateGLuintObjects(int gSolverGetParticlesSize){
    // For Particles
    for (int i = 0; i < gSolverGetParticlesSize; i++) {
        GLuint newGVertexArrayObject = 0;
        gVertexArrayObjects.push_back(newGVertexArrayObject);
        GLuint newGVertexBufferObject = 0;
        gVertexBufferObjects.push_back(newGVertexBufferObject);
        GLuint newGIndexBufferObject = 0;
        gIndexBufferObjects.push_back(newGIndexBufferObject);

        // Push GLuint objects to their respective vectors
        glGenVertexArrays(1, &newGVertexBufferObject);
        glBindVertexArray(newGVertexBufferObject);
        glGenBuffers(1, &newGVertexBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, newGVertexBufferObject);
    }
}

void Sphere::GenerateGLuintLight(){
    // For Light
    glGenVertexArrays(1, &lightVertexBufferObject);
    glBindVertexArray(lightVertexBufferObject);
    glGenBuffers(1, &lightVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, lightVertexBufferObject);
}

// For the box that will contain particles
void Sphere::GenerateGluintBoxObjects(){
    glGenVertexArrays(1, &boxVertexBufferObject);
    glBindVertexArray(boxVertexBufferObject);
    glGenBuffers(1, &boxVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, boxVertexBufferObject);
}

void Sphere::GenerateModelBufferData(int gSolverGetParticlesSize){
    GenerateParticleModelData(); // This creates a particle "blueprint"

    // Create vertex data lists for each particle
    for (int i = 0; i < gSolverGetParticlesSize; i++) {
        std::vector<GLfloat> vertexData_i = getVerticesAndAddColorData();
        gVertexData.push_back(vertexData_i);
    }

    // Fix indices information from 0 - n to 1 - n
    offsetGModelIndices();
    //printGModelIndices();

    gTotalIndices = gModelIndices.size();

    // Send rendering data to buffers for each particle
    for (int i = 0; i < gSolverGetParticlesSize; i++) {

        glGenVertexArrays(1, &gVertexArrayObjects[i]);
        glBindVertexArray(gVertexArrayObjects[i]);

        glBufferData(GL_ARRAY_BUFFER, 								// Kind of buffer we are working with 
                                                                    // (e.g. GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER)
                    gVertexData[i].size() * sizeof(GL_FLOAT), 	// Size of data in bytes
                    gVertexData[i].data(), 						// Raw array of data
                    GL_STATIC_DRAW);								// How we intend to use the data

        // Generate EBO
        glGenBuffers(1, &gIndexBufferObjects[i]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferObjects[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, gModelIndices.size() * sizeof(GLuint), gModelIndices.data(), GL_STATIC_DRAW);

        ConfigureVertexAttributes();
    }
}

void Sphere::GenerateParticleModelData(){
    std::string gModelFilepath = "/Users/natashadaas/ParticleSimulation/3D/src/models/sphereCorrect.obj";
    ParseModelData(gModelFilepath); 

    getModelMesh();
}

void Sphere::ParseModelData(std::string filepath){
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

void Sphere::getModelMesh() {
    outFile << "--- In getModelMesh() ---" << std::endl;

    outFile << "-------" << std::endl;
    outFile << "Collecting Indices into Triangles" << std::endl;
    outFile << "-------" << std::endl;

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

    outFile << "-------" << std::endl;
    outFile<< "End Collecting Indices into Triangles" << std::endl;
    outFile << "-------" << std::endl;

   outFile << "--- Exiting getModelMesh() ---" << std::endl;
}

std::vector<GLfloat> Sphere::getVerticesAndAddColorData() {
    std::vector<GLfloat> vertexPositionsAndColor;

    outFile << "--- In getVerticesAndAddColorData() ---" << std::endl;

    for (int i = 0; i < gModelVertices.size(); i++) {
        vertexPositionsAndColor.push_back(gModelVertices[i].coordinates.x);
        vertexPositionsAndColor.push_back(gModelVertices[i].coordinates.y);
        vertexPositionsAndColor.push_back(gModelVertices[i].coordinates.z);
        outFile << "Index: " << i << std::endl;
        outFile << "Vertex: (" << gModelVertices[i].coordinates.x << "," << gModelVertices[i].coordinates.y << "," << gModelVertices[i].coordinates.z << ")" << std::endl;
        vertexPositionsAndColor.push_back(0.0f);
        vertexPositionsAndColor.push_back(0.3f);
        vertexPositionsAndColor.push_back(0.7f);
        vertexPositionsAndColor.push_back(gModelNormals[gModelNormalsMap[i]].coordinates.x);
        vertexPositionsAndColor.push_back(gModelNormals[gModelNormalsMap[i]].coordinates.y);
        vertexPositionsAndColor.push_back(gModelNormals[gModelNormalsMap[i]].coordinates.z);
        outFile << "gModelNormalsMap[i]: " << gModelNormalsMap[i] << std::endl;
        outFile << "Normal: (" << gModelNormals[gModelNormalsMap[i]].coordinates.x << "," << gModelNormals[gModelNormalsMap[i]].coordinates.y << "," << gModelNormals[gModelNormalsMap[i]].coordinates.z << ")" << std::endl;
        
    }

    outFile << "--- Exiting getVerticesAndAddColorData() ---" << std::endl;

    return vertexPositionsAndColor;
}

void Sphere::offsetGModelIndices() {
    for (int i = 0; i < gModelIndices.size(); i++) {
        gModelIndices[i] = gModelIndices[i] - 1; 
    } 
}

void Sphere::ConfigureVertexAttributes() {
   // Enable the vertex attribute for position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 9, (GLvoid*)0);

    // Enable the vertex attribute for color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 9, (GLvoid*)(sizeof(GL_FLOAT) * 3));

    // Enable the vertex attribute for normal
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 9, (GLvoid*)(sizeof(GL_FLOAT) * 6));

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void Sphere::GenerateLightBufferData(){
    // Create vertex data lists for light particle
    std::vector<GLfloat> vertexData_light = getVerticesAndAddColorData();

    glGenVertexArrays(1, &lightVertexArrayObject);
    glBindVertexArray(lightVertexArrayObject);
    glBufferData(GL_ARRAY_BUFFER, 								// Kind of buffer we are working with 
                                                                // (e.g. GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER)
                vertexData_light.size() * sizeof(GL_FLOAT), 	// Size of data in bytes
                vertexData_light.data(), 						// Raw array of data
                GL_STATIC_DRAW);								// How we intend to use the data

    // Generate EBO
    glGenBuffers(1, &lightIndexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightIndexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, gModelIndices.size() * sizeof(GLuint), gModelIndices.data(), GL_STATIC_DRAW);

    ConfigureLightVertexAttributes();
}

void Sphere::ConfigureLightVertexAttributes() {
   // Enable the vertex attribute for position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 9, (GLvoid*)0);

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
}

void Sphere::GenerateBoxBufferData(){
    GenerateBoxModelData(); // Creates a "box" blueprint

    std::vector<GLfloat> boxVertexData = getBoxVerticesAndAddColorData();
   
    // Fix indices information from 0 - n to 1 - n
    offsetGBoxIndices();

    gTotalBoxIndices = gModelBoxIndices.size();

    glGenVertexArrays(1, &boxVertexArrayObject);
    glBindVertexArray(boxVertexArrayObject);
    glBufferData(GL_ARRAY_BUFFER, 								// Kind of buffer we are working with 
                                                                // (e.g. GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER)
                boxVertexData.size() * sizeof(GL_FLOAT), 	// Size of data in bytes
                boxVertexData.data(), 						// Raw array of data
                GL_STATIC_DRAW);	

    // Generate EBO
    glGenBuffers(1, &boxIndexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxIndexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, gModelBoxIndices.size() * sizeof(GLuint), gModelBoxIndices.data(), GL_STATIC_DRAW);

    ConfigureBoxVertexAttributes();
}

void Sphere::offsetGBoxIndices(){
    for (int i = 0; i < gModelBoxIndices.size(); i++) {
        gModelBoxIndices[i] = gModelBoxIndices[i] - 1; 
    } 
}

void Sphere::GenerateBoxModelData(){
    std::string gModelFilepath = "/Users/natashadaas/ParticleSimulation/3D/src/models/cube.obj";
    ParseBoxModelData(gModelFilepath); 
    getBoxModelMesh();
}

void Sphere::ParseBoxModelData(std::string filepath){
    outFile << "--- In ParseBoxModelData() ---" << std::endl;
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
            gModelBoxVertices.push_back(v);
        }
        if (words[0] == "vn") {
            Vertex v(glm::vec3(std::stof(words[1]), std::stof(words[2]), std::stof(words[3])));
            v.printVertex("Normal");
            gModelBoxNormals.push_back(v);
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
            gModelBoxIndices.push_back(indices[0]);
            gModelBoxIndices.push_back(indices[1]);
            gModelBoxIndices.push_back(indices[2]);
            gModelBoxIndices.push_back(normals[0]);
            gModelBoxIndices.push_back(normals[1]);
            gModelBoxIndices.push_back(normals[2]);
            gModelBoxNormalsMap[indices[0] - 1] = normals[0] - 1;
            gModelBoxNormalsMap[indices[1] - 1] = normals[1] - 1;
            gModelBoxNormalsMap[indices[2] - 1] = normals[2] - 1;
        }
    }

    // Close the file
    inputFile.close();
    outFile << "--- Exiting parseModelData() ---" << std::endl;
}

void Sphere::getBoxModelMesh(){
    outFile << "--- In getBoxModelMesh() ---" << std::endl;

    outFile << "-------" << std::endl;
    outFile << "Collecting Indices into Triangles" << std::endl;
    outFile << "-------" << std::endl;

    for (int i = 0; i < gModelBoxIndices.size(); i++) {
        
        outFile << "Triangle" << std::endl;

        outFile << "  Index 1: " << gModelBoxIndices[i] << std::endl;
        outFile << "  Index 2: " << gModelBoxIndices[i+1] << std::endl;
        outFile << "  Index 3: " << gModelBoxIndices[i+2] << std::endl;
        outFile << "  Normal 1: " << gModelBoxNormalsMap[gModelBoxIndices[i] - 1] + 1<< std::endl;
        outFile << "  Normal 2: " << gModelBoxNormalsMap[gModelBoxIndices[i+1] - 1] + 1<< std::endl;
        outFile << "  Normal 3: " << gModelBoxNormalsMap[gModelBoxIndices[i+2] - 1] + 1<< std::endl;

        // print Vertices of Triangle
        outFile << "  Vertices: " << std::endl;
        gModelBoxVertices[gModelBoxIndices[i] - 1].printVertex("Vertex");
        gModelBoxVertices[gModelBoxIndices[i + 1] - 1].printVertex("Vertex");
        gModelBoxVertices[gModelBoxIndices[i + 2] - 1].printVertex("Vertex");

        // print Normals of Triangle
        outFile << "  Normals: " << std::endl;
        gModelBoxVertices[gModelBoxIndices[i + 3] - 1].printVertex("Normal");
        gModelBoxVertices[gModelBoxIndices[i + 4] - 1].printVertex("Normal");
        gModelBoxVertices[gModelBoxIndices[i + 5] - 1].printVertex("Normal");

        Triangle t(gModelBoxVertices[gModelBoxIndices[i] - 1], gModelBoxVertices[gModelBoxIndices[i + 1] - 1], gModelBoxVertices[gModelBoxIndices[i + 2] - 1], gModelBoxVertices[gModelBoxIndices[i + 3] - 1], gModelBoxVertices[gModelBoxIndices[i + 4] - 1], gModelBoxVertices[gModelBoxIndices[i + 5] - 1]);
        i = i + 5;
        gBoxMesh.push_back(t);
    }

    outFile << "-------" << std::endl;
    outFile<< "End Collecting Indices into Triangles" << std::endl;
    outFile << "-------" << std::endl;

   outFile << "--- Exiting getBoxModelMesh() ---" << std::endl;
}

std::vector<GLfloat> Sphere::getBoxVerticesAndAddColorData(){
    std::vector<GLfloat> vertexPositionsAndColor;

    outFile << "--- In getBoxVerticesAndAddColorData() ---" << std::endl;

    for (int i = 0; i < gModelBoxVertices.size(); i++) {
        vertexPositionsAndColor.push_back(gModelBoxVertices[i].coordinates.x);
        vertexPositionsAndColor.push_back(gModelBoxVertices[i].coordinates.y);
        vertexPositionsAndColor.push_back(gModelBoxVertices[i].coordinates.z);
        outFile << "Index: " << i << std::endl;
        outFile << "Vertex: (" << gModelBoxVertices[i].coordinates.x << "," << gModelBoxVertices[i].coordinates.y << "," << gModelBoxVertices[i].coordinates.z << ")" << std::endl;
        vertexPositionsAndColor.push_back(0.0f);
        vertexPositionsAndColor.push_back(0.3f);
        vertexPositionsAndColor.push_back(0.7f);
        vertexPositionsAndColor.push_back(gModelBoxNormals[gModelBoxNormalsMap[i]].coordinates.x);
        vertexPositionsAndColor.push_back(gModelBoxNormals[gModelBoxNormalsMap[i]].coordinates.y);
        vertexPositionsAndColor.push_back(gModelBoxNormals[gModelBoxNormalsMap[i]].coordinates.z);
        outFile << "gModelBoxNormalsMap[i]: " << gModelBoxNormalsMap[i] << std::endl;
        outFile << "Normal: (" << gModelBoxNormals[gModelBoxNormalsMap[i]].coordinates.x << "," << gModelBoxNormals[gModelBoxNormalsMap[i]].coordinates.y << "," << gModelBoxNormals[gModelBoxNormalsMap[i]].coordinates.z << ")" << std::endl;
        
    }

    outFile << "--- Exiting getBoxVerticesAndAddColorData() ---" << std::endl;

    return vertexPositionsAndColor;
}

void Sphere::ConfigureBoxVertexAttributes(){
    // Enable the vertex attribute for position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 9, (GLvoid*)0);

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
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

void Sphere::CleanUp(int gSolverGetParticlesSize){
    for (int i = 0; i < gSolverGetParticlesSize; i++){
        glDeleteBuffers(1, &gVertexBufferObjects[i]);
        glDeleteVertexArrays(1, &gVertexArrayObjects[i]);
    }

    glDeleteBuffers(1, &lightVertexArrayObject);
    glDeleteBuffers(1, &lightVertexBufferObject);
    glDeleteBuffers(1, &boxVertexBufferObject);
}

int Sphere::getTotalIndices(){
    return gTotalIndices;
}

int Sphere::getBoxTotalIndices(){
    return gTotalBoxIndices;
}

std::vector<GLuint> Sphere::getGVertexArrayObjects(){
    return gVertexArrayObjects;
}

std::vector<GLuint> Sphere::getGVertexBufferObjects(){
    return gVertexBufferObjects;
}

GLuint* Sphere::getLightVertexArrayObject(){
    return &lightVertexArrayObject;
}

GLuint* Sphere::getLightVertexBufferObject(){
    return &lightVertexBufferObject;
}

GLuint* Sphere::getBoxVertexArrayObject(){
return &boxVertexArrayObject;
}

GLuint* Sphere::getBoxVertexBufferObject(){
    return &boxVertexBufferObject;
}