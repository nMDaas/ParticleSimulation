/* Compilation on Linux: 
 g++ -std=c++17 ./src/*.cpp -o prog -I ./include/ -I./../common/thirdparty/ -lSDL2 -ldl
*/

// Third Party Libraries
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> 

// C++ Standard Template Library (STL)
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>

// Our libraries
#include "Camera.hpp"
#include "Triangle.hpp"
#include "Vertex.hpp"
#include "Particle.hpp"
#include "Solver.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "Sphere.hpp"

// vvvvvvvvvvvvvvvvvvvvvvvvvv Globals vvvvvvvvvvvvvvvvvvvvvvvvvv
// Globals generally are prefixed with 'g' in this application.

// Screen Dimensions
int gScreenWidth 						= 640;
int gScreenHeight 						= 480;
SDL_Window* gGraphicsApplicationWindow 	= nullptr;
SDL_GLContext gOpenGLContext			= nullptr;

bool gQuit = false;

/*
GLuint gGraphicsPipelineShaderProgram	= 0;
GLuint gGraphicsLighterPipelineShaderProgram	= 0;

std::vector<GLuint> gVertexArrayObjects;
std::vector<GLuint> gVertexBufferObjects;
std::vector<GLuint> gIndexBufferObjects;*/

std::vector<Particle> gParticles;

// Solver information
Solver gSolver;

// Camera information
Camera gCamera;

// Sphere information 
Sphere gSphere;

// Scene information 
Scene gScene(&gSolver, &gCamera, &gSphere);

// Renderer information
Renderer gRenderer(gScreenWidth, gScreenHeight, &gScene);



// Model information for particle ("sphere")
/*
std::vector<Vertex> gModelVertices;
std::vector<Vertex> gModelNormals;
std::unordered_map<int, int> gModelNormalsMap;
std::vector<GLfloat> gModelInformation;
std::vector<int> gModelIndices;
std::vector<Triangle> gMesh;
size_t gTotalIndices = 0;*/

// Light information
Particle gLightParticle(glm::vec3(3.0f,1.0f,5.0f), 1.0f);
/*GLuint lightVertexArrayObject = 0;
GLuint lightVertexBufferObject = 0;
GLuint lightIndexBufferObject = 0;*/

//std::vector<std::vector<GLfloat>> gVertexData;

std::ofstream outFile("output.txt");

bool  g_rotatePositive=true;
float g_uRotate=0.0f;

static void GLClearAllErrors(){
    while(glGetError() != GL_NO_ERROR){
    }
}

static bool GLCheckErrorStatus(const char* function, int line){
    while(GLenum error = glGetError()){
        std::cout << "OpenGL Error:" << error 
                  << "\tLine: " << line 
                  << "\tfunction: " << function << std::endl;
        return true;
    }
    return false;
}

#define GLCheck(x) GLClearAllErrors(); x; GLCheckErrorStatus(#x,__LINE__);

// Initialization of the graphics application
// Typically this will involve setting up a window and the OpenGL Context (with the appropriate version)
void InitializeProgram(){
	// Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO)< 0){
		std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
		exit(1);
	}
	
	// Setup the OpenGL Context
	// Use OpenGL 4.1 core or greater
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
	// We want to request a double buffer for smooth updating.
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// Create an application window using OpenGL that supports SDL
	gGraphicsApplicationWindow = SDL_CreateWindow( "OpenGL First Program",
													SDL_WINDOWPOS_UNDEFINED,
													SDL_WINDOWPOS_UNDEFINED,
													gScreenWidth,
													gScreenHeight,
													SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

	// Check if Window did not create.
	if( gGraphicsApplicationWindow == nullptr ){
		std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << "\n";
		exit(1);
	}

	// Create an OpenGL Graphics Context
	gOpenGLContext = SDL_GL_CreateContext( gGraphicsApplicationWindow );
	if( gOpenGLContext == nullptr){
		std::cout << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << "\n";
		exit(1);
	}

	// Initialize GLAD Library
	if(!gladLoadGLLoader(SDL_GL_GetProcAddress)){
		std::cout << "glad did not initialize" << std::endl;
		exit(1);
	}
	
}

/*
void ConfigureVertexAttributes() {
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

void ConfigureLightVertexAttributes() {
   // Enable the vertex attribute for position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 9, (GLvoid*)0);

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
}

void parseModelData(std::string filepath){
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

void getModelMesh() {
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

std::vector<GLfloat> getVerticesAndAddColorData() {
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

void offsetGModelIndices() {
    for (int i = 0; i < gModelIndices.size(); i++) {
        gModelIndices[i] = gModelIndices[i] - 1; 
    } 
}

void printGModelIndices() {
    for (int i = 0; i < gModelIndices.size(); i++) {
        std::cout << gModelIndices[i] << std::endl;
    } 
}*/

// For debugging purposes
void printAllVertexInformation(std::vector<float> vertices) {

    std::cout << "-------" << std::endl;
    std::cout << "printAllVertexInformation(): " << vertices.size() << std::endl;
    std::cout << "-------" << std::endl;

    for (int i = 0; i < vertices.size(); i++) {
        std::cout << "Vertex " << std::endl;
        std::cout << "  position: (" << vertices[i] << "," << vertices[i+1] << "," << vertices[i+2] << ")" << std::endl;
        std::cout << "  color: (" << vertices[i+3] << "," << vertices[i+4] << "," << vertices[i+5] << ")" << std::endl;
        i = i + 5;
    }

    std::cout << "-------" << std::endl;
    std::cout << "end printAllVertexInformation()" << std::endl;
    std::cout << "-------" << std::endl;
}

/*
void GenerateParticleModelData(){
    std::string gModelFilepath = "/Users/natashadaas/ParticleSimulation/3D/src/models/sphereCorrect.obj";
    parseModelData(gModelFilepath); 

    gModelVertices = gSphere.getModelVertices();
    gModelNormals = gSphere.getModelNormals();
    gModelNormalsMap = gSphere.getModelNormalsMap();
    gModelIndices = gSphere.getModelIndices();

    getModelMesh();
}

void GenerateModelBufferData(){
    GenerateParticleModelData(); // This creates a particle "blueprint"

    // Create vertex data lists for each particle
    for (int i = 0; i < gSolver.getParticles().size(); i++) {
        std::vector<GLfloat> vertexData_i = getVerticesAndAddColorData();
        gVertexData.push_back(vertexData_i);
    }

    // Fix indices information from 0 - n to 1 - n
    offsetGModelIndices();
    //printGModelIndices();

    gTotalIndices = gModelIndices.size();

    // Send rendering data to buffers for each particle
    for (int i = 0; i < gSolver.getParticles().size(); i++) {

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

void GenerateLightBufferData(){
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

// Generate newGVertexArrayObject, newGVertexBufferObject and newGIndexBufferObject for each particle
void GenerateGLuintObjects(){
    // For Particles
    for (int i = 0; i < gSolver.getParticles().size(); i++) {
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

void GenerateGLuintLight(){
    // For Light
    glGenVertexArrays(1, &lightVertexBufferObject);
    glBindVertexArray(lightVertexBufferObject);
    glGenBuffers(1, &lightVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, lightVertexBufferObject);
}

void VertexSpecification(){

    GenerateGLuintObjects();

    GenerateGLuintLight();

    GenerateModelBufferData();

    GenerateLightBufferData();
}*/

/**
* Helper Function to get OpenGL Version Information
*
* @return void
*/
void getOpenGLVersionInfo(){
  std::cout << "Vendor: " << glGetString(GL_VENDOR) << "\n";
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";
  std::cout << "Version: " << glGetString(GL_VERSION) << "\n";
  std::cout << "Shading language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
}


/**
* Function called in the Main application loop to handle user input
*
* @return void
*/
void Input(){
	// Event handler that handles various events in SDL
	// that are related to input and output
	SDL_Event e;
	//Handle events on queue
	while(SDL_PollEvent( &e ) != 0){
		// If users posts an event to quit
		// An example is hitting the "x" in the corner of the window.
		if(e.type == SDL_QUIT){
			std::cout << "Goodbye! (Leaving MainApplicationLoop())" << std::endl;
			gQuit = true;
		}
        if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE){
			std::cout << "ESC: Goodbye! (Leaving MainApplicationLoop())" << std::endl;
            gQuit = true;
        }
	}

    // Retrieve keyboard state
    const Uint8 *state = SDL_GetKeyboardState(NULL);

    // Camera
    // Update our position of the camera
    if (state[SDL_SCANCODE_W]) {
        gCamera.MoveForward(0.1f);
    }
    if (state[SDL_SCANCODE_S]) {
        gCamera.MoveBackward(0.1f);
    }
    if (state[SDL_SCANCODE_A]) {
        gCamera.MoveLeft(0.1f);
    }
    if (state[SDL_SCANCODE_D]) {
        gCamera.MoveRight(0.1f);
    }
}

/**
* The last function called in the program
* This functions responsibility is to destroy any global
* objects in which we have create dmemory.
*
* @return void
*/
void CleanUp(){
	//Destroy our SDL2 Window
	SDL_DestroyWindow(gGraphicsApplicationWindow );
	gGraphicsApplicationWindow = nullptr;

    // Delete our OpenGL Objects
    gSphere.CleanUp(gSolver.getParticles().size());

	// Delete our Graphics pipeline
    gRenderer.CleanUp();

	//Quit SDL subsystems
	SDL_Quit();
}


/**
* The entry point into our C++ programs.
*
* @return program status
*/
int main( int argc, char* args[] ){
    std::cout << "Press ESC to quit\n";

	// Setup the graphics program
	InitializeProgram();

    gScene.SetupSolverAndLights();
	
	// Setup geometry (for particles and lights)
    gSphere.VertexSpecification(gSolver.getParticles().size());

    //gScene.setLightGLuints(&lightVertexArrayObject, &lightVertexBufferObject);
    //gScene.setParticleGLuints(gVertexArrayObjects, gVertexBufferObjects);
	
    gRenderer.CreateGraphicsPipelines();

    int gTotalIndices = gSphere.getTotalIndices();
	
	// While application is running
	while(!gQuit){
		Input(); // Handle Input

        gRenderer.RenderScene(gTotalIndices);

		//Update screen of our specified window
		SDL_GL_SwapWindow(gGraphicsApplicationWindow);
		SDL_Delay(16); 
	}

	// Call the cleanup function when our program terminates
	CleanUp();

	return 0;
}