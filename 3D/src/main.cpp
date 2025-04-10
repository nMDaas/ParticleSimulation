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

// Our libraries
#include "Camera.hpp"
#include "Triangle.hpp"
#include "Vertex.hpp"

// vvvvvvvvvvvvvvvvvvvvvvvvvv Globals vvvvvvvvvvvvvvvvvvvvvvvvvv
// Globals generally are prefixed with 'g' in this application.

// Screen Dimensions
int gScreenWidth 						= 640;
int gScreenHeight 						= 480;
SDL_Window* gGraphicsApplicationWindow 	= nullptr;
SDL_GLContext gOpenGLContext			= nullptr;

bool gQuit = false;

GLuint gGraphicsPipelineShaderProgram	= 0;

GLuint gVertexArrayObject					= 0;
GLuint gVertexArrayObjectFloor= 0;
GLuint 	gVertexBufferObject					= 0;
GLuint  gVertexBufferObjectFloor            = 0;
GLuint 	gIndexBufferObject                  = 0;
GLuint 	gIndexBufferObjectFloor             = 0;

std::vector<GLuint> gVertexArrayObjects;
std::vector<GLuint> gVertexBufferObjects;
std::vector<GLuint> gIndexBufferObjects;

// Model information 
std::vector<Vertex> gModelVertices;
std::vector<GLfloat> gModelInformation;
std::vector<int> gModelIndices;
std::vector<Triangle> gMesh;
size_t gTotalIndices = 0;

bool  g_rotatePositive=true;
float g_uRotate=0.0f;

Camera gCamera;

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

std::string LoadShaderAsString(const std::string& filename){
    std::string result = "";

    std::string line = "";
    std::ifstream myFile(filename.c_str());

    if(myFile.is_open()){
        while(std::getline(myFile, line)){
            result += line + '\n';
        }
        myFile.close();

    }

    return result;
}

GLuint CompileShader(GLuint type, const std::string& source){
	// Compile our shaders
	GLuint shaderObject;

	// Based on the type passed in, we create a shader object specifically for that
	// type.
	if(type == GL_VERTEX_SHADER){
		shaderObject = glCreateShader(GL_VERTEX_SHADER);
	}else if(type == GL_FRAGMENT_SHADER){
		shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	}

	const char* src = source.c_str();
	// The source of our shader
	glShaderSource(shaderObject, 1, &src, nullptr);
	// Now compile our shader
	glCompileShader(shaderObject);

	// Retrieve the result of our compilation
	int result;
	// Our goal with glGetShaderiv is to retrieve the compilation status
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &result);

	if(result == GL_FALSE){
		int length;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
		char* errorMessages = new char[length]; // Could also use alloca here.
		glGetShaderInfoLog(shaderObject, length, &length, errorMessages);

		if(type == GL_VERTEX_SHADER){
			std::cout << "ERROR: GL_VERTEX_SHADER compilation failed!\n" << errorMessages << "\n";
		}else if(type == GL_FRAGMENT_SHADER){
			std::cout << "ERROR: GL_FRAGMENT_SHADER compilation failed!\n" << errorMessages << "\n";
		}
		// Reclaim our memory
		delete[] errorMessages;

		// Delete our broken shader
		glDeleteShader(shaderObject);

		return 0;
	}

  return shaderObject;
}



/**
* Creates a graphics program object (i.e. graphics pipeline) with a Vertex Shader and a Fragment Shader
*
* @param vertexShaderSource Vertex source code as a string
* @param fragmentShaderSource Fragment shader source code as a string
* @return id of the program Object
*/
GLuint CreateShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource){

    // Create a new program object
    GLuint programObject = glCreateProgram();

    // Compile our shaders
    GLuint myVertexShader   = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint myFragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    // Link our two shader programs together.
	// Consider this the equivalent of taking two .cpp files, and linking them into
	// one executable file.
    glAttachShader(programObject,myVertexShader);
    glAttachShader(programObject,myFragmentShader);
    glLinkProgram(programObject);

    // Validate our program
    glValidateProgram(programObject);

    // Once our final program Object has been created, we can
	// detach and then delete our individual shaders.
    glDetachShader(programObject,myVertexShader);
    glDetachShader(programObject,myFragmentShader);
	// Delete the individual shaders once we are done
    glDeleteShader(myVertexShader);
    glDeleteShader(myFragmentShader);

    return programObject;
}


/**
* Create the graphics pipeline
*
* @return void
*/
void CreateGraphicsPipeline(){

    std::string vertexShaderSource      = LoadShaderAsString("./shaders/vert.glsl");
    std::string fragmentShaderSource    = LoadShaderAsString("./shaders/frag.glsl");

	gGraphicsPipelineShaderProgram = CreateShaderProgram(vertexShaderSource,fragmentShaderSource);
}


/**
* Initialization of the graphics application. Typically this will involve setting up a window
* and the OpenGL Context (with the appropriate version)
*
* @return void
*/
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

void ConfigureVertexAttributes() {
   // Enable the vertex attribute for position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 6, (GLvoid*)0);

    // Enable the vertex attribute for color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 6, (GLvoid*)(sizeof(GL_FLOAT) * 3));


	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void parseModelData(std::string filepath){
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
            v.printVertex();
            gModelVertices.push_back(v);
        }
         if (words[0] == "f") {
            int index;
            std::vector<int> indices;
            for (int i = 1; i <=3; i++) {
                std::stringstream ss(words[i]);
                ss >> index;
                indices.push_back(index);
            }

            std::cout << "Indices: " << indices[0] << "," << indices[1] << "," << indices[2] << std::endl;
            gModelIndices.push_back(indices[0]);
            gModelIndices.push_back(indices[1]);
            gModelIndices.push_back(indices[2]);
        }

    }

    // Close the file
    inputFile.close();
}

void getModelMesh() {
    //std::vector<Triangle> triangleModelMesh;

    std::cout << "-------" << std::endl;
    std::cout << "Collecting Indices into Triangles" << std::endl;
    std::cout << "-------" << std::endl;

    for (int i = 0; i < gModelIndices.size(); i++) {
        
        std::cout << "Triangle" << std::endl;

        std::cout << "  Index 1: " << gModelIndices[i] << std::endl;
        std::cout << "  Index 2: " << gModelIndices[i+1] << std::endl;
        std::cout << "  Index 3: " << gModelIndices[i+2] << std::endl;

        // print Vertices of Triangle
        std::cout << "  Vertices: " << std::endl;
        gModelVertices[gModelIndices[i] - 1].printVertex();
        gModelVertices[gModelIndices[i + 1] - 1].printVertex();
        gModelVertices[gModelIndices[i + 2] - 1].printVertex();

         Triangle t(gModelVertices[gModelIndices[i] - 1], gModelVertices[gModelIndices[i + 1] - 1], gModelVertices[gModelIndices[i + 2] - 1]);
         i = i + 2;
         gMesh.push_back(t);
    }

    std::cout << "-------" << std::endl;
    std::cout << "End Collecting Indices into Triangles" << std::endl;
    std::cout << "-------" << std::endl;

    //return triangleModelMesh;
}

std::vector<GLfloat> getVerticesAndAddColorData() {
    std::vector<GLfloat> vertexPositionsAndColor;

    for (int i = 0; i < gModelVertices.size(); i++) {
        vertexPositionsAndColor.push_back(gModelVertices[i].coordinates.x);
        vertexPositionsAndColor.push_back(gModelVertices[i].coordinates.y);
        vertexPositionsAndColor.push_back(gModelVertices[i].coordinates.z);
        vertexPositionsAndColor.push_back(0.0f);
        vertexPositionsAndColor.push_back(0.3f);
        vertexPositionsAndColor.push_back(0.7f);
    }

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
}

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

void GenerateModelBufferData(){
    std::string gModelFilepath = "/Users/natashadaas/ParticleSimulation/3D/src/models/sphere.obj";
    parseModelData(gModelFilepath); 

    getModelMesh();

    std::vector<GLfloat> vertexData = getVerticesAndAddColorData();

    offsetGModelIndices();

    printGModelIndices();

    printAllVertexInformation(vertexData);

    gTotalIndices = gModelIndices.size();

    for (int i = 0; i < gVertexArrayObjects.size(); i++) {

        glGenVertexArrays(1, &gVertexArrayObjects[i]);
        glBindVertexArray(gVertexArrayObjects[i]);

        glBufferData(GL_ARRAY_BUFFER, 								// Kind of buffer we are working with 
                                                                    // (e.g. GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER)
                    vertexData.size() * sizeof(GL_FLOAT), 	// Size of data in bytes
                    vertexData.data(), 						// Raw array of data
                    GL_STATIC_DRAW);								// How we intend to use the data

        // Generate EBO
        glGenBuffers(1, &gIndexBufferObjects[i]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferObjects[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, gModelIndices.size() * sizeof(GLuint), gModelIndices.data(), GL_STATIC_DRAW);

    }
}

void VertexSpecification(){

    for (int i = 0; i < 1; i++) {
        GLuint newGVertexArrayObject = 0;
        gVertexArrayObjects.push_back(newGVertexArrayObject);
        GLuint newGVertexBufferObject = 0;
        gVertexBufferObjects.push_back(newGVertexBufferObject);
        GLuint newGIndexBufferObject = 0;
        gIndexBufferObjects.push_back(newGIndexBufferObject);

        glGenVertexArrays(1, &newGVertexBufferObject);
        glBindVertexArray(newGVertexBufferObject);
        glGenBuffers(1, &newGVertexBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, newGVertexBufferObject);
    }

    GenerateModelBufferData();

    ConfigureVertexAttributes();
}

/**
* Setup your geometry during the vertex specification step
*
* @return void
*/
void VertexSpecification1(){

	// Geometry Data
	// Here we are going to store x,y, and z position attributes within vertexPositons for the data.
	// For now, this information is just stored in the CPU, and we are going to store this data
	// on the GPU shortly, in a call to glBufferData which will store this information into a
	// vertex buffer object.
	// Note: That I have segregated the data from the OpenGL calls which follow in this function.
	//       It is not strictly necessary, but I find the code is cleaner if OpenGL (GPU) related
	//       functions are packed closer together versus CPU operations.
	const std::vector<GLfloat> vertexData
	{
        // 0 - Vertex
		-0.5f, -0.5f, 0.0f, 	// Left vertex position
		1.0f,  0.0f, 0.0f, 	    // color
        // 1 - Vertex
		0.5f, -0.5f, 0.0f,  	// right vertex position
		0.0f,  1.0f, 0.0f,  	// color
        // 2 - Vertex
		-0.5f,  0.5f, 0.0f,  	// Top left vertex position
		0.0f,  0.0f, 1.0f,  	// color
        // 3 - Vertex
		0.5f,  0.5f, 0.0f,  	// Top-right position
		0.0f,  0.0f, 1.0f,  	// color
	};

	// Vertex Arrays Object (VAO) Setup
	// Note: We can think of the VAO as a 'wrapper around' all of the Vertex Buffer Objects,
	//       in the sense that it encapsulates all VBO state that we are setting up.
	//       Thus, it is also important that we glBindVertexArray (i.e. select the VAO we want to use)
	//       before our vertex buffer object operations.
	glGenVertexArrays(1, &gVertexArrayObject);
	// We bind (i.e. select) to the Vertex Array Object (VAO) that we want to work withn.
	glBindVertexArray(gVertexArrayObject);

	// Vertex Buffer Object (VBO) creation
	// Create a new vertex buffer object
	// Note:  We’ll see this pattern of code often in OpenGL of creating and binding to a buffer.
	glGenBuffers(1, &gVertexBufferObject);
	// Next we will do glBindBuffer.
	// Bind is equivalent to 'selecting the active buffer object' that we want to
	// work with in OpenGL.
	glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
	// Now, in our currently binded buffer, we populate the data from our
	// 'vertexPositions' (which is on the CPU), onto a buffer that will live
	// on the GPU.
	glBufferData(GL_ARRAY_BUFFER, 						// Kind of buffer we are working with 
														// (e.g. GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER)
				 vertexData.size() * sizeof(GL_FLOAT), 	// Size of data in bytes
				 vertexData.data(), 					// Raw array of data
				 GL_STATIC_DRAW);						// How we intend to use the data
 
    // Index buffer data for a quad
    const std::vector<GLuint> indexBufferData {2,0,1, 3,2,1};
    // Setup the Index Buffer Object (IBO i.e. EBO)
    glGenBuffers(1,&gIndexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                 gIndexBufferObject);
    // Populate our Index Buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indexBufferData.size()*sizeof(GLuint),
                 indexBufferData.data(),
                 GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);

    glVertexAttribPointer(0,  		// Attribute 0 corresponds to the enabled glEnableVertexAttribArray
							  		// In the future, you'll see in our vertex shader this also correspond
							  		// to (layout=0) which selects these attributes.
                          3,  		// The number of components (e.g. x,y,z = 3 components)
                          GL_FLOAT, // Type
                          GL_FALSE, // Is the data normalized
                          sizeof(GL_FLOAT)*6, 		// Stride
                         (void*)0	// Offset
    );


    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          3, 
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(GL_FLOAT)*6,
                          (GLvoid*)(sizeof(GL_FLOAT)*3)
            );


	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void VertexSpecification2(){

	const std::vector<GLfloat> vertexDataFloor
	{
        // 0 - Vertex
		-5.0f, -1.0f, -5.0f, 	// Left vertex position
		1.0f,  0.0f, 0.0f, 	    // color
        // 1 - Vertex
		5.0f,  -1.0f, -5.0f,  	// right vertex position
		0.0f,  1.0f, 0.0f,  	// color
        // 2 - Vertex
		-5.0f, -1.0f, 5.0f,  	// Top left vertex position
		0.0f,  1.0f, 0.0f,  	// color
        // 3 - Vertex
		5.0f,  -1.0f, 5.0f,  	// Top-right position
		1.0f,  0.0f, 0.0f,  	// color
	};

	glGenVertexArrays(1, &gVertexArrayObjectFloor);

	glBindVertexArray(gVertexArrayObjectFloor);

	glGenBuffers(1, &gVertexBufferObjectFloor);

	glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObjectFloor);

	glBufferData(GL_ARRAY_BUFFER, 						// Kind of buffer we are working with 
														// (e.g. GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER)
				 vertexDataFloor.size() * sizeof(GL_FLOAT), 	// Size of data in bytes
				 vertexDataFloor.data(), 					    // Raw array of data
				 GL_STATIC_DRAW);						        // How we intend to use the data
 
    const std::vector<GLuint> indexBufferData {2,0,1, 3,2,1};

    glGenBuffers(1,&gIndexBufferObjectFloor);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                 gIndexBufferObjectFloor);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indexBufferData.size()*sizeof(GLuint),
                 indexBufferData.data(),
                 GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,  		// Attribute 0 corresponds to the enabled glEnableVertexAttribArray
							  		// In the future, you'll see in our vertex shader this also correspond
							  		// to (layout=0) which selects these attributes.
                          3,  		// The number of components (e.g. x,y,z = 3 components)
                          GL_FLOAT, // Type
                          GL_FALSE, // Is the data normalized
                          sizeof(GL_FLOAT)*6, 		// Stride
                         (void*)0	// Offset
    );

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          3, 
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(GL_FLOAT)*6,
                          (GLvoid*)(sizeof(GL_FLOAT)*3)
            );

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}


void DrawParticle(int i){
    glBindVertexArray(gVertexArrayObjects[i]);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObjects[i]);
    glDrawElements(GL_TRIANGLES,gTotalIndices,GL_UNSIGNED_INT,0);
    glUseProgram(0);
}

void PreDrawParticle(int i){
    // Use our shader
	glUseProgram(gGraphicsPipelineShaderProgram);

    // Model transformation by translating our object into world space
    glm::mat4 model = glm::translate(glm::mat4(1.0f),glm::vec3(-5.0f,0.0f,-5.0f)); 

    // Update our model matrix by applying a rotation after our translation
    model           = glm::rotate(model,glm::radians(g_uRotate),glm::vec3(0.0f,1.0f,0.0f));

	// TA_README: Send data to GPU    
	// Note: the error keeps showing up until you actually USE u_ModelMatrix in vert.glsl
	GLint u_ModelMatrixLocation = glGetUniformLocation( gGraphicsPipelineShaderProgram,"u_ModelMatrix");
    if(u_ModelMatrixLocation >=0){
        glUniformMatrix4fv(u_ModelMatrixLocation,1,GL_FALSE,&model[0][0]);
    }else{
        std::cout << "Could not find u_ModelMatrix, maybe a mispelling?\n";
        exit(EXIT_FAILURE);
    }

    // Projection matrix (in perspective) 
    glm::mat4 perspective = glm::perspective(glm::radians(45.0f),
                                             (float)gScreenWidth/(float)gScreenHeight,
                                             0.1f,
                                             20.0f);
	// TA_README: Send data to GPU
	// Note: the error keeps showing up until you actually USE u_Projection in vert.glsl
	GLint u_ProjectionLocation= glGetUniformLocation( gGraphicsPipelineShaderProgram,"u_Projection");
    if(u_ProjectionLocation>=0){
        glUniformMatrix4fv(u_ProjectionLocation,1,GL_FALSE,&perspective[0][0]);
    }else{
        std::cout << "Could not find u_Perspective, maybe a mispelling?\n";
        exit(EXIT_FAILURE);
    }
}

void PreDrawAndDraw(){
    glEnable(GL_DEPTH_TEST);                    
    glDisable(GL_CULL_FACE);

    // Initialize clear color
    // This is the background of the screen.
    glViewport(0, 0, gScreenWidth, gScreenHeight);
    glClearColor( 0.1f, 4.f, 7.f, 1.f );

    //Clear color buffer and Depth Buffer
  	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < gVertexArrayObjects.size(); i++) {
        PreDrawParticle(i);

        // Update the View Matrix
        GLint u_ViewMatrixLocation = glGetUniformLocation(gGraphicsPipelineShaderProgram,"u_ViewMatrix");
        if(u_ViewMatrixLocation>=0){
            glm::mat4 viewMatrix = gCamera.GetViewMatrix();
            glUniformMatrix4fv(u_ViewMatrixLocation,1,GL_FALSE,&viewMatrix[0][0]);
        }else{
            std::cout << "Could not find u_ModelMatrix, maybe a mispelling?\n";
            exit(EXIT_FAILURE);
        }

        DrawParticle(i);
    }
}

void PreDraw(){
    glEnable(GL_DEPTH_TEST);                    
    glDisable(GL_CULL_FACE);

    // Initialize clear color
    // This is the background of the screen.
    glViewport(0, 0, gScreenWidth, gScreenHeight);
    glClearColor( 0.1f, 4.f, 7.f, 1.f );

    //Clear color buffer and Depth Buffer
  	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // Use our shader
	glUseProgram(gGraphicsPipelineShaderProgram);

    // Model transformation by translating our object into world space
    glm::mat4 model = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,0.0f,-5.0f)); 

    // Update our model matrix by applying a rotation after our translation
    model           = glm::rotate(model,glm::radians(g_uRotate),glm::vec3(0.0f,1.0f,0.0f));

	// TA_README: Send data to GPU    
	// Note: the error keeps showing up until you actually USE u_ModelMatrix in vert.glsl
	GLint u_ModelMatrixLocation = glGetUniformLocation( gGraphicsPipelineShaderProgram,"u_ModelMatrix");
    if(u_ModelMatrixLocation >=0){
        glUniformMatrix4fv(u_ModelMatrixLocation,1,GL_FALSE,&model[0][0]);
    }else{
        std::cout << "Could not find u_ModelMatrix, maybe a mispelling?\n";
        exit(EXIT_FAILURE);
    }

    // Projection matrix (in perspective) 
    glm::mat4 perspective = glm::perspective(glm::radians(45.0f),
                                             (float)gScreenWidth/(float)gScreenHeight,
                                             0.1f,
                                             20.0f);
	// TA_README: Send data to GPU
	// Note: the error keeps showing up until you actually USE u_Projection in vert.glsl
	GLint u_ProjectionLocation= glGetUniformLocation( gGraphicsPipelineShaderProgram,"u_Projection");
    if(u_ProjectionLocation>=0){
        glUniformMatrix4fv(u_ProjectionLocation,1,GL_FALSE,&perspective[0][0]);
    }else{
        std::cout << "Could not find u_Perspective, maybe a mispelling?\n";
        exit(EXIT_FAILURE);
    }

    // Perform our rotation update
    if(g_rotatePositive){
        g_uRotate+=0.5f;
    }else{
        g_uRotate-=0.5f;
    }
}

void PreDrawFloor(){
    // Use our shader
	glUseProgram(gGraphicsPipelineShaderProgram);

    // Model transformation by translating our object into world space
    glm::mat4 model = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,0.0f,-5.0f)); 

    // Retrieve our location of our Model Matrix
    GLint u_ModelMatrixLocation = glGetUniformLocation( gGraphicsPipelineShaderProgram,"u_ModelMatrix");
    if(u_ModelMatrixLocation >=0){
        glUniformMatrix4fv(u_ModelMatrixLocation,1,GL_FALSE,&model[0][0]);
    }else{
        std::cout << "Could not find u_ModelMatrix, maybe a mispelling?\n";
        exit(EXIT_FAILURE);
    }


    // Update the View Matrix
    GLint u_ViewMatrixLocation = glGetUniformLocation(gGraphicsPipelineShaderProgram,"u_ViewMatrix");
    if(u_ViewMatrixLocation>=0){
        glm::mat4 viewMatrix = gCamera.GetViewMatrix();
        glUniformMatrix4fv(u_ViewMatrixLocation,1,GL_FALSE,&viewMatrix[0][0]);
    }else{
        std::cout << "Could not find u_ModelMatrix, maybe a mispelling?\n";
        exit(EXIT_FAILURE);
    }


    // Projection matrix (in perspective) 
    glm::mat4 perspective = glm::perspective(glm::radians(45.0f),
                                             (float)gScreenWidth/(float)gScreenHeight,
                                             0.1f,
                                             20.0f);

    // Retrieve our location of our perspective matrix uniform 
    GLint u_ProjectionLocation= glGetUniformLocation( gGraphicsPipelineShaderProgram,"u_Projection");
    if(u_ProjectionLocation>=0){
        glUniformMatrix4fv(u_ProjectionLocation,1,GL_FALSE,&perspective[0][0]);
    }else{
        std::cout << "Could not find u_Perspective, maybe a mispelling?\n";
        exit(EXIT_FAILURE);
    }

}



/**
* Draw
* The render function gets called once per loop.
* Typically this includes 'glDraw' related calls, and the relevant setup of buffers
* for those calls.
*
* @return void
*/
void Draw(){
    // Enable our attributes
	glBindVertexArray(gVertexArrayObjects[0]);

	// Select the vertex buffer object we want to enable
    glBindBuffer(GL_ARRAY_BUFFER, gVertexArrayObjects[0]);

    //Render data
    glDrawElements(GL_TRIANGLES,
                    gTotalIndices,
                    GL_UNSIGNED_INT,
                    0);

	// Stop using our current graphics pipeline
	// Note: This is not necessary if we only have one graphics pipeline.
    glUseProgram(0);
}

void DrawFloor(){
	glBindVertexArray(gVertexArrayObjectFloor);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObjectFloor);
    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
    glUseProgram(0);
}

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
    // Two static variables to hold the mouse position
    static int mouseX=gScreenWidth/2;
    static int mouseY=gScreenHeight/2; 

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
        if(e.type==SDL_MOUSEMOTION){
            // Capture the change in the mouse position
            mouseX+=e.motion.xrel;
            mouseY+=e.motion.yrel;
            std::cout << mouseX << "," << mouseY << std::endl;
            gCamera.MouseLook(mouseX,mouseY);
        }
	}

    // Retrieve keyboard state
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_UP]) {
    }
    if (state[SDL_SCANCODE_DOWN]) {
    }
    if (state[SDL_SCANCODE_LEFT]) {
        g_rotatePositive=false;
        std::cout << "g_rotatePositive: " << g_rotatePositive << std::endl;
    }
    if (state[SDL_SCANCODE_RIGHT]) {
        g_rotatePositive=true;
        std::cout << "g_rotatePositive: " << g_rotatePositive << std::endl;
    }

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
* Main Application Loop
* This is an infinite loop in our graphics application
*
* @return void
*/
void MainLoop(){

    // Little trick to map mouse to center of screen always.
    // Useful for handling 'mouselook'
    // This works because we effectively 're-center' our mouse at the start
    // of every frame prior to detecting any mouse motion.
    SDL_WarpMouseInWindow(gGraphicsApplicationWindow,gScreenWidth/2,gScreenHeight/2);
    SDL_SetRelativeMouseMode(SDL_TRUE);


	// While application is running
	while(!gQuit){
		// Handle Input
		Input();
        PreDrawAndDraw();
        //PreDraw();
        //Draw();
        //PreDrawFloor();
		//PreDrawAndDraw();
		//Update screen of our specified window
		SDL_GL_SwapWindow(gGraphicsApplicationWindow);
		SDL_Delay(16); // TA_README: This is to reduce the speed of rotation in certain computers
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
    glDeleteBuffers(1, &gVertexBufferObject);
    glDeleteVertexArrays(1, &gVertexArrayObject);

	// Delete our Graphics pipeline
    glDeleteProgram(gGraphicsPipelineShaderProgram);

	//Quit SDL subsystems
	SDL_Quit();
}


/**
* The entry point into our C++ programs.
*
* @return program status
*/
int main( int argc, char* args[] ){
    std::cout << "Use wasd keys to move mouse to rotate\n";
    std::cout << "Press ESC to quit\n";

	// 1. Setup the graphics program
	InitializeProgram();
	
	// 2. Setup our geometry
    VertexSpecification();
	//VertexSpecification1();
	//VertexSpecification2();
	
	// 3. Create our graphics pipeline
	// 	- At a minimum, this means the vertex and fragment shader
	CreateGraphicsPipeline();
	
	// 4. Call the main application loop
	MainLoop();	

	// 5. Call the cleanup function when our program terminates
	CleanUp();

	return 0;
}
