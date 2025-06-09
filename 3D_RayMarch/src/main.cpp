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
#include <chrono>
#include <thread>

// Our libraries
#include "Camera.hpp"
#include "Triangle.hpp"
#include "Vertex.hpp"
#include "Particle.hpp"
#include "Solver.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "ModelProcessor.hpp"

// vvvvvvvvvvvvvvvvvvvvvvvvvv Globals vvvvvvvvvvvvvvvvvvvvvvvvvv
// Globals generally are prefixed with 'g' in this application.

// Screen Dimensions
int gScreenWidth 						= 640;
int gScreenHeight 						= 480;
SDL_Window* gGraphicsApplicationWindow 	= nullptr;
SDL_GLContext gOpenGLContext			= nullptr;

bool gQuit = false;

std::vector<Particle> gParticles;

// Core Classes/Objects in Scene
Solver gSolver;
Camera gCamera;
ModelProcessor gModelProcessor;
Scene gScene(&gSolver, &gCamera, &gModelProcessor);
Renderer gRenderer(gScreenWidth, gScreenHeight, &gScene);

// Core Variables for Scene
// note: fluid_restitution, wall_restitution, threshold and velocity damping affect how the particles react
// # = 15, size = 0.6f, fluid_r = 0.5f, wall_r - 0.9f, damping = 0.98f
// # = 25, size = 0.4f, fluid_r = 1.2f, wall_r - 0.1f, damping = 0.7f
int gNumParticles = 120;
int gParticleIndexToActivate = 0; // index of next particle to activate
float gParticleSize = 0.2f;

bool gPause = false;
int gCounter = 0;

bool  g_rotatePositive=true;
float g_uRotate=0.0f;

// vvvvvvvvvvvvvvvvvvvvvvvvvv OpenGL Boilerplate vvvvvvvvvvvvvvvvvvvvvvvvvv

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

// Helper Function to get OpenGL Version Information
void getOpenGLVersionInfo(){
  std::cout << "Vendor: " << glGetString(GL_VENDOR) << "\n";
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";
  std::cout << "Version: " << glGetString(GL_VERSION) << "\n";
  std::cout << "Shading language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
}

// vvvvvvvvvvvvvvvvvvvvvvvvvv Most Relevant main() Functions vvvvvvvvvvvvvvvvvvvvvvvvvv

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

// Function called in the Main application loop to handle user input
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

		if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
			
            case SDLK_ESCAPE:
                std::cout << "ESC: Goodbye! (Leaving MainApplicationLoop())" << std::endl;
                gQuit = true;
                break;
            /*case SDLK_LEFT:
                gRenderer.updateZ(1.0f);
				gScene.updateBoxRotationZ(1.0f);
                break;
            case SDLK_RIGHT:
                // Handle right arrow
				gRenderer.updateZ(-1.0f);
				gScene.updateBoxRotationZ(-1.0f);
                break;*/
            case SDLK_UP:
                std::cout << "Up arrow pressed" << std::endl;
                // Handle up arrow
                break;
            case SDLK_DOWN:
                std::cout << "Down arrow pressed" << std::endl;
                // Handle down arrow
                break;
			case SDLK_SPACE:
				gPause = !gPause;
				std::cout << "Space key pressed!" << std::endl;
				// Do something when space is pressed
				break;
        }
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
	if (state[SDL_SCANCODE_LEFT]) {
        gRenderer.updateZ(1.0f);
		gScene.updateBoxRotationZ(1.0f);
    }
    if (state[SDL_SCANCODE_RIGHT]) {
        gRenderer.updateZ(-1.0f);
		gScene.updateBoxRotationZ(-1.0f);
    }
}

void CleanUp(){
	//Destroy our SDL2 Window
	SDL_DestroyWindow(gGraphicsApplicationWindow );
	gGraphicsApplicationWindow = nullptr;

    // Delete our OpenGL Objects
    gModelProcessor.CleanUp(gSolver.getParticles().size());

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

	// Clock setup so particles can be steadily released
	using clock = std::chrono::steady_clock;
	auto lastActionTime = clock::now();
    const std::chrono::milliseconds interval(500); // 1 second

	// Setup the graphics program
	InitializeProgram();

    gScene.SetupScene(gNumParticles, gParticleSize);

    gRenderer.CreateGraphicsPipelines();

	gRenderer.VertexSpecification();
	
	// While application is running
	while(!gQuit){
		
		auto now = clock::now();

		//gSolver.printSolverInfo();


		if (!gPause) {
			// Activate a new particle
			if (gParticleIndexToActivate < gNumParticles) {
				if (now - lastActionTime >= interval) {
					std::cout << "Activating one particle at: "
							<< std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count()
							<< " ms\n";
					
					gSolver.activateNewParticle(gParticleIndexToActivate);

					lastActionTime = now;

					gParticleIndexToActivate++;
				}
			}
		}

		Input(); // Handle Input

		

		if (!gPause) {
			gSolver.update(gScene.getBox(), gCounter); // TODO should be getGBox

			//gRenderer.RenderScene();

			gRenderer.RenderScene_RayMarch();

			//Update screen of our specified window
			SDL_GL_SwapWindow(gGraphicsApplicationWindow);
		}
		
		SDL_Delay(16); 

		// Sleep to reduce CPU usage
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		gCounter++;
	}

	// Call the cleanup function when our program terminates
	CleanUp();

	return 0;
}