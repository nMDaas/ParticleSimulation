#include <SDL2/SDL.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include "Camera.hpp"
#include "Triangle.hpp"
#include "Vertex.hpp"
#include "Particle.hpp"

// ------------------------ Globals ------------------------
// Globals generally are prefixed with 'g' in this application.

// Screen Dimensions
int gScreenWidth 						= 640;
int gScreenHeight 						= 480;
SDL_Window* gGraphicsApplicationWindow 	= nullptr;
SDL_GLContext gOpenGLContext			= nullptr;

// Main loop flag
bool gQuit = false; // If this is quit = 'true' then the program terminates.

GLuint gGraphicsPipelineShaderProgram	= 0;

// OpenGL Objects
GLuint gVertexArrayObject = 0; // Vertex Array Object (VAO)
GLuint  gVertexBufferObject = 0; // Vertex Buffer Object (VBO)
GLuint gElementBufferObject = 0; // EBO

Camera gCamera; // Camera

GLenum gPolygonMode = GL_LINE; // Draw wireframe mode

// Model information 
std::vector<Vertex> gModelVertices;
std::vector<GLfloat> gModelInformation;
std::vector<int> gModelIndices;
std::vector<Triangle> gMesh;
size_t gTotalIndices = 0;

std::vector<Particle> gParticles;
std::string gModelFilepath;

// ------------------------ Globals ------------------------

// ------------------------ Error Handling Routines ------------------------
static void GLClearAllErrors(){
    while(glGetError() != GL_NO_ERROR){
    }
}

// Returns true if we have an error
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
// ------------------------ Error Handling Routines ------------------------

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
	GLuint shaderObject;

	if(type == GL_VERTEX_SHADER){
		shaderObject = glCreateShader(GL_VERTEX_SHADER);
	}else if(type == GL_FRAGMENT_SHADER){
		shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	}

	const char* src = source.c_str();
	glShaderSource(shaderObject, 1, &src, nullptr);
	glCompileShader(shaderObject);

	int result;
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &result);

	if(result == GL_FALSE){
		int length;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
		char* errorMessages = new char[length]; 
		glGetShaderInfoLog(shaderObject, length, &length, errorMessages);

		if(type == GL_VERTEX_SHADER){
			std::cout << "ERROR: GL_VERTEX_SHADER compilation failed!\n" << errorMessages << "\n";
		}else if(type == GL_FRAGMENT_SHADER){
			std::cout << "ERROR: GL_FRAGMENT_SHADER compilation failed!\n" << errorMessages << "\n";
		}
		delete[] errorMessages;

		glDeleteShader(shaderObject);

		return 0;
	}

  return shaderObject;
}

GLuint CreateShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource){

    GLuint programObject = glCreateProgram();

    GLuint myVertexShader   = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint myFragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    glAttachShader(programObject,myVertexShader);
    glAttachShader(programObject,myFragmentShader);
    glLinkProgram(programObject);

    glValidateProgram(programObject);

    glDetachShader(programObject,myVertexShader);
    glDetachShader(programObject,myFragmentShader);
    glDeleteShader(myVertexShader);
    glDeleteShader(myFragmentShader);

    return programObject;
}

void CreateGraphicsPipeline(){

    std::string vertexShaderSource      = LoadShaderAsString("./shaders/vert.glsl");
    std::string fragmentShaderSource    = LoadShaderAsString("./shaders/frag.glsl");

	gGraphicsPipelineShaderProgram = CreateShaderProgram(vertexShaderSource,fragmentShaderSource);
}

void InitializeProgram(){
	if(SDL_Init(SDL_INIT_VIDEO)< 0){
		std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
		exit(1);
	}
	
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	gGraphicsApplicationWindow = SDL_CreateWindow( "Tesselation",
													SDL_WINDOWPOS_UNDEFINED,
													SDL_WINDOWPOS_UNDEFINED,
													gScreenWidth,
													gScreenHeight,
													SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

	if( gGraphicsApplicationWindow == nullptr ){
		std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << "\n";
		exit(1);
	}

	gOpenGLContext = SDL_GL_CreateContext( gGraphicsApplicationWindow );
	if( gOpenGLContext == nullptr){
		std::cout << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << "\n";
		exit(1);
	}

	if(!gladLoadGLLoader(SDL_GL_GetProcAddress)){
		std::cout << "glad did not initialize" << std::endl;
		exit(1);
	}
	
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

void GenerateModelBufferData(){
    parseModelData(gModelFilepath); 

    getModelMesh();

    std::vector<GLfloat> vertexData = getVerticesAndAddColorData();

    offsetGModelIndices();

    printGModelIndices();

    printAllVertexInformation(vertexData);

    /*const std::vector<GLfloat> vertexData
	{
		 -0.8f, -0.8f,  0.0f, 1.0f, 0.0f, 0.0f,// Vertex 1: Bottom-left
        0.8f, -0.8f,  0.0f, 1.0f, 0.0f, 0.0f,// Vertex 2: Bottom-right
        0.8f,  0.8f,  0.0f, 1.0f, 0.0f, 0.0f, // Vertex 3: Top-right
        -0.8f,  0.8f,  0.0f, 1.0f, 0.0f, 0.0f
       
	};

    const std::vector<GLuint> indices{
        0, 1, 2, // First triangle
        2, 3, 0  // Second triangle
    };*/

    gTotalIndices = gModelIndices.size();

    glGenVertexArrays(1, &gVertexArrayObject);
	glBindVertexArray(gVertexArrayObject);

    glBufferData(GL_ARRAY_BUFFER, 								// Kind of buffer we are working with 
																// (e.g. GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER)
				 vertexData.size() * sizeof(GL_FLOAT), 	// Size of data in bytes
				 vertexData.data(), 						// Raw array of data
				 GL_STATIC_DRAW);								// How we intend to use the data

    // Generate EBO
    glGenBuffers(1, &gElementBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gElementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, gModelIndices.size() * sizeof(GLuint), gModelIndices.data(), GL_STATIC_DRAW);
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

void VertexSpecification(){

	glGenVertexArrays(1, &gVertexBufferObject);
	glBindVertexArray(gVertexBufferObject);
	glGenBuffers(1, &gVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);

    GenerateModelBufferData();

    ConfigureVertexAttributes();
}

void PreDraw(){
    glEnable(GL_DEPTH_TEST);                    
    glDisable(GL_CULL_FACE);

    glPolygonMode(GL_FRONT_AND_BACK,gPolygonMode);

    glViewport(0, 0, gScreenWidth, gScreenHeight);
    glClearColor( 0.1f, 4.f, 7.f, 1.f );

  	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // Use our shader
	glUseProgram(gGraphicsPipelineShaderProgram);

    // Model transformation by translating our object into world space
    glm::mat4 model = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,0.0f,0.0f)); 


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

void Draw(){
	glBindVertexArray(gVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER,gVertexBufferObject);

    glDrawElements(GL_TRIANGLES, gTotalIndices, // number of indices, not triangles
        GL_UNSIGNED_INT, // type of indices array
        nullptr // offset pointer to the data
        );
        glUseProgram(0);

    glUseProgram(0);
}

void getOpenGLVersionInfo(){
  std::cout << "Vendor: " << glGetString(GL_VENDOR) << "\n";
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";
  std::cout << "Version: " << glGetString(GL_VERSION) << "\n";
  std::cout << "Shading language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
}

void SetUpParticles(){
    Particle newParticle(glm::vec3(0,0,0), 0.0f); // currently setting up dummy values
    gParticles.push_back(newParticle);
}

void SetUpVertexData(){
    for (int i = 0; i < gParticles.size(); i++) {
        gModelFilepath = "/Users/natashadaas/CS4300TAPrep/Assignment06_ModelParser/part2/src/models/sphere.obj";
        gModelVertices.clear();
        gModelInformation.clear();
        gModelIndices.clear();
        gMesh.clear();
        GenerateModelBufferData();
        ConfigureVertexAttributes();
    }
}

void Input(){
    static int mouseX=gScreenWidth/2;
    static int mouseY=gScreenHeight/2; 

	SDL_Event e;
	while(SDL_PollEvent( &e ) != 0){
            if(e.type == SDL_QUIT){
                //std::cout << "Goodbye! (Leaving MainApplicationLoop())" << std::endl;
                gQuit = true;
            }
            if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE){
                //std::cout << "ESC: Goodbye! (Leaving MainApplicationLoop())" << std::endl;
                gQuit = true;
            }
            if(e.type==SDL_MOUSEMOTION){
                mouseX+=e.motion.xrel;
                mouseY+=e.motion.yrel;
                gCamera.MouseLook(mouseX,mouseY);
            }
    }
	

    const Uint8 *state = SDL_GetKeyboardState(NULL);

    // Camera
    if (state[SDL_SCANCODE_W]) {
        gCamera.MoveForward(0.01f);
    }
    if (state[SDL_SCANCODE_S]) {
        gCamera.MoveBackward(0.01f);
    }
    if (state[SDL_SCANCODE_A]) {
    }
    if (state[SDL_SCANCODE_D]) {
    }
}

void MainLoop(){

    SDL_WarpMouseInWindow(gGraphicsApplicationWindow,gScreenWidth/2,gScreenHeight/2);

    SetUpParticles();

    SetUpVertexData();

	while(!gQuit){
		Input();  
		PreDraw();
		Draw();
		SDL_GL_SwapWindow(gGraphicsApplicationWindow);
	}
}

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

int main( int argc, char* args[] ){
    //std::cout << "Use w and s keys to move forward and back\n";
    //std::cout << "Use up and down to change tessellation\n";
    //std::cout << "Use tab to toggle wireframe\n";
    //std::cout << "Press ESC to quit\n";

	InitializeProgram();
	
	VertexSpecification();
	
	CreateGraphicsPipeline();
	
	MainLoop();	

	CleanUp();

	return 0;
}
