#include "Renderer.hpp"

Renderer::Renderer(){}

Renderer::Renderer(int i_screenWidth, int i_screenHeight, Scene* scene){
    screenWidth = i_screenWidth;
    screenHeight = i_screenHeight;
    mainScene = scene;
}

void Renderer::CreateGraphicsPipelines(){
    std::string vertexShaderSource      = LoadShaderAsString("./shaders/vertLight.glsl");
    std::string fragmentShaderSource    = LoadShaderAsString("./shaders/fragLight.glsl");

    gGraphicsLighterPipelineShaderProgram = CreateShaderProgram(vertexShaderSource,fragmentShaderSource);
}

std::string Renderer::LoadShaderAsString(const std::string& filename){
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

GLuint Renderer::CreateShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource){
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

GLuint Renderer::CompileShader(GLuint type, const std::string& source){
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

void Renderer::RenderScene(int gTotalIndices, Camera gCamera) {
    PreDraw();
    DrawLights(gTotalIndices, gCamera);
}

void Renderer::PreDraw() {
    glEnable(GL_DEPTH_TEST);                    
    glDisable(GL_CULL_FACE);

    // Initialize clear color
    // This is the background of the screen.
    glViewport(0, 0, screenWidth, screenHeight);
    glClearColor( 0.1f, 4.f, 7.f, 1.f );

    //Clear color buffer and Depth Buffer
  	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void Renderer::DrawLights(int gTotalIndices, Camera gCamera){
    PreDrawLight();

    GLint u_ViewMatrixLocation = glGetUniformLocation(gGraphicsLighterPipelineShaderProgram,"u_ViewMatrix");
    if(u_ViewMatrixLocation>=0){
        glm::mat4 viewMatrix = gCamera.GetViewMatrix();
        glUniformMatrix4fv(u_ViewMatrixLocation,1,GL_FALSE,&viewMatrix[0][0]);
    }else{
        std::cout << "Could not find u_ModelMatrix, maybe a mispelling?\n";
        exit(EXIT_FAILURE);
    }

    DrawLight(gTotalIndices);
}

void Renderer::PreDrawLight() {
    Particle* gLightParticle = mainScene->getLights()[0];

    // Use our shader
	glUseProgram(gGraphicsLighterPipelineShaderProgram);

    // Model transformation by translating our object into world space
    float r = gLightParticle->getRadius();
    glm::mat4 model = glm::translate(glm::mat4(1.0f), gLightParticle->getPosition());
    //model = glm::rotate(model, glm::radians(g_uRotate), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(r, r, r));

	// TA_README: Send data to GPU    
	// Note: the error keeps showing up until you actually USE u_ModelMatrix in vert.glsl
	GLint u_ModelMatrixLocation = glGetUniformLocation( gGraphicsLighterPipelineShaderProgram,"u_ModelMatrix");
    if(u_ModelMatrixLocation >=0){
        glUniformMatrix4fv(u_ModelMatrixLocation,1,GL_FALSE,&model[0][0]);
    }else{
        std::cout << "Could not find u_ModelMatrix, maybe a mispelling?\n";
        exit(EXIT_FAILURE);
    }

    // Projection matrix (in perspective) 
    glm::mat4 perspective = glm::perspective(glm::radians(45.0f),
                                             (float)screenWidth/(float)screenHeight,
                                             0.1f,
                                             10000.0f);
	// TA_README: Send data to GPU
	// Note: the error keeps showing up until you actually USE u_Projection in vert.glsl
	GLint u_ProjectionLocation= glGetUniformLocation( gGraphicsLighterPipelineShaderProgram,"u_Projection");
    if(u_ProjectionLocation>=0){
        glUniformMatrix4fv(u_ProjectionLocation,1,GL_FALSE,&perspective[0][0]);
    }else{
        std::cout << "Could not find u_Perspective, maybe a mispelling?\n";
        exit(EXIT_FAILURE);
    }
}

void Renderer::DrawLight(int gTotalIndices){
    GLuint lightVertexArrayObject = *(mainScene->getLightVertexArrayObject());
    GLuint lightVertexBufferObject = *(mainScene->getLightVertexBufferObject());

    glBindVertexArray(lightVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, lightVertexBufferObject);
    glDrawElements(GL_TRIANGLES,gTotalIndices,GL_UNSIGNED_INT,0);
    glUseProgram(0);
}