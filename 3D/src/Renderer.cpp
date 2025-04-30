#include "Renderer.hpp"

Renderer::Renderer(){}

Renderer::Renderer(int i_screenWidth, int i_screenHeight, Scene* scene){
    screenWidth = i_screenWidth;
    screenHeight = i_screenHeight;
    mainScene = scene;
}

void Renderer::CreateGraphicsPipelines(){

    std::string vertexShaderSource      = LoadShaderAsString("./shaders/vertPhong.glsl");
    std::string fragmentShaderSource    = LoadShaderAsString("./shaders/fragPhong.glsl");

	gGraphicsPipelineShaderProgram = CreateShaderProgram(vertexShaderSource,fragmentShaderSource);


    std::string vertexShaderSource_light      = LoadShaderAsString("./shaders/vertLight.glsl");
    std::string fragmentShaderSource_light    = LoadShaderAsString("./shaders/fragLight.glsl");

    gGraphicsLighterPipelineShaderProgram = CreateShaderProgram(vertexShaderSource_light,fragmentShaderSource_light);
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

void Renderer::RenderScene(int gTotalIndices, int gLightTotalIndices, int gBoxTotalIndices) {
    mainScene->InitializeGLuints();
    PreDraw();
    DrawParticles(gTotalIndices);
    DrawLights(gLightTotalIndices);
    DrawBox(gBoxTotalIndices);
    std::cout << std::endl;
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

void Renderer::DrawParticles(int gTotalIndices){
    for (int i = 0; i < mainScene->getGVertexArrayObjects_map()["Particle"].size(); i++) {
        PreDrawParticle(i);

        // Update the View Matrix
        GLint u_ViewMatrixLocation = glGetUniformLocation(gGraphicsPipelineShaderProgram,"u_ViewMatrix");
        if(u_ViewMatrixLocation>=0){
            glm::mat4 viewMatrix = mainScene->getCamera()->GetViewMatrix();
            glUniformMatrix4fv(u_ViewMatrixLocation,1,GL_FALSE,&viewMatrix[0][0]);
        }else{
            std::cout << "Could not find u_ModelMatrix, maybe a mispelling?\n";
            exit(EXIT_FAILURE);
        }

        DrawParticle(i, gTotalIndices);
    }
    std::cout << std::endl;
}

void Renderer::PreDrawParticle(int i){
    // Use our shader
	glUseProgram(gGraphicsPipelineShaderProgram);

    // Model transformation by translating our object into world space
    float r = mainScene->getSolver()->getParticles()[i]->getRadius();
    glm::mat4 model = glm::translate(glm::mat4(1.0f), mainScene->getSolver()->getParticles()[i]->getPosition());
    //model = glm::rotate(model, glm::radians(g_uRotate), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(r, r, r));

	// Note: the error keeps showing up until you actually USE u_ModelMatrix in vert.glsl
	GLint u_ModelMatrixLocation = glGetUniformLocation( gGraphicsPipelineShaderProgram,"u_ModelMatrix");
    if(u_ModelMatrixLocation >=0){
        glUniformMatrix4fv(u_ModelMatrixLocation,1,GL_FALSE,&model[0][0]);
    }else{
        std::cout << "Could not find u_ModelMatrix, maybe a mispelling?\n";
        exit(EXIT_FAILURE);
    }

    GLint i_lightColor = glGetUniformLocation( gGraphicsPipelineShaderProgram,"i_lightColor");
    if(i_lightColor >=0){
        glUniform3fv(i_lightColor, 1, &glm::vec3(1.0f, 1.0f, 1.0f)[0]);
    }else{
        std::cout << "Could not find i_lightColor, maybe a mispelling?\n";
        exit(EXIT_FAILURE);
    }

    Particle* gLightParticle = mainScene->getLights()[0];
    GLint i_lightPosition = glGetUniformLocation( gGraphicsPipelineShaderProgram,"i_lightPosition");
    if(i_lightPosition >=0){
        glUniform3fv(i_lightPosition, 1, &gLightParticle->getPosition()[0]);
    }else{
        std::cout << "Could not find i_lightPosition, maybe a mispelling?\n";
        exit(EXIT_FAILURE);
    }

    GLint i_viewPos = glGetUniformLocation( gGraphicsPipelineShaderProgram,"i_viewPos");
    if(i_viewPos >=0){
        glUniform3fv(i_viewPos, 1, &mainScene->getCamera()->GetCameraEyePosition()[0]);
    }else{
        std::cout << "Could not find i_viewPos, maybe a mispelling?\n";
        exit(EXIT_FAILURE);
    }


    // Projection matrix (in perspective) 
    glm::mat4 perspective = glm::perspective(glm::radians(45.0f),
                                             (float)screenWidth/(float)screenHeight,
                                             0.1f,
                                             10000.0f);
	// Note: the error keeps showing up until you actually USE u_Projection in vert.glsl
	GLint u_ProjectionLocation= glGetUniformLocation( gGraphicsPipelineShaderProgram,"u_Projection");
    if(u_ProjectionLocation>=0){
        glUniformMatrix4fv(u_ProjectionLocation,1,GL_FALSE,&perspective[0][0]);
    }else{
        std::cout << "Could not find u_Perspective, maybe a mispelling?\n";
        exit(EXIT_FAILURE);
    }
    std::cout << std::endl;
}

void Renderer::DrawParticle(int i, int gTotalIndices){
    glBindVertexArray(mainScene->getGVertexArrayObjects_map()["Particle"][i]);
    glBindBuffer(GL_ARRAY_BUFFER, mainScene->getGVertexBufferObjects_map()["Particle"][i]);
    glDrawElements(GL_TRIANGLES,gTotalIndices,GL_UNSIGNED_INT,0);
    glUseProgram(0);
    std::cout << std::endl;
}

void Renderer::DrawLights(int gTotalIndices){
    std::cout << "-- In DrawLights -- " << std::endl;
    PreDrawLight();

    GLint u_ViewMatrixLocation = glGetUniformLocation(gGraphicsLighterPipelineShaderProgram,"u_ViewMatrix");
    if(u_ViewMatrixLocation>=0){
        glm::mat4 viewMatrix = mainScene->getCamera()->GetViewMatrix();
        glUniformMatrix4fv(u_ViewMatrixLocation,1,GL_FALSE,&viewMatrix[0][0]);
    }else{
        std::cout << "Could not find u_ModelMatrix, maybe a mispelling?\n";
        exit(EXIT_FAILURE);
    }

    DrawLight(gTotalIndices);
    std::cout << "-- Exiting DrawLights -- " << std::endl;
    std::cout << std::endl;
}

void Renderer::PreDrawLight() {
    std::cout << "-- In PreDrawLight -- " << std::endl;
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
    std::cout << "-- Exiting PreDrawLight -- " << std::endl;
    std::cout << std::endl;
}

void Renderer::DrawLight(int gTotalIndices){
    std::cout << "-- In DrawLight -- " << std::endl;
    GLuint lightVertexArrayObject = mainScene->getGVertexArrayObjects_map()["Light"][0];
    GLuint lightVertexBufferObject = mainScene->getGVertexBufferObjects_map()["Light"][0];

    glBindVertexArray(lightVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, lightVertexBufferObject);
    glDrawElements(GL_TRIANGLES,gTotalIndices,GL_UNSIGNED_INT,0);
    glUseProgram(0);
    std::cout << "-- Exiting DrawLight -- " << std::endl;
    std::cout << std::endl;
}

void Renderer::DrawBox(int gBoxTotalIndices){
    PreDrawBox();

    GLint u_ViewMatrixLocation = glGetUniformLocation(gGraphicsLighterPipelineShaderProgram,"u_ViewMatrix");
    if(u_ViewMatrixLocation>=0){
        glm::mat4 viewMatrix = mainScene->getCamera()->GetViewMatrix();
        glUniformMatrix4fv(u_ViewMatrixLocation,1,GL_FALSE,&viewMatrix[0][0]);
    }else{
        std::cout << "Could not find u_ModelMatrix, maybe a mispelling?\n";
        exit(EXIT_FAILURE);
    }

    DrawBoxActually(gBoxTotalIndices);
    std::cout << std::endl;
}

void Renderer::PreDrawBox(){
    // Use our shader
	glUseProgram(gGraphicsLighterPipelineShaderProgram);

    // Model transformation by translating our object into world space
    float r = 0.2f;
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    //model = glm::rotate(model, glm::radians(g_uRotate), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(r, r, r));

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
	// Note: the error keeps showing up until you actually USE u_Projection in vert.glsl
	GLint u_ProjectionLocation= glGetUniformLocation( gGraphicsLighterPipelineShaderProgram,"u_Projection");
    if(u_ProjectionLocation>=0){
        glUniformMatrix4fv(u_ProjectionLocation,1,GL_FALSE,&perspective[0][0]);
    }else{
        std::cout << "Could not find u_Perspective, maybe a mispelling?\n";
        exit(EXIT_FAILURE);
    }
    std::cout << std::endl;
}

void Renderer::DrawBoxActually(int gBoxTotalIndices){
    GLuint boxVertexArrayObject = mainScene->getGVertexArrayObjects_map()["Box"][0];
    GLuint boxVertexBufferObject = mainScene->getGVertexArrayObjects_map()["Box"][0];

    glBindVertexArray(boxVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, boxVertexArrayObject);
    glDrawElements(GL_TRIANGLES,gBoxTotalIndices,GL_UNSIGNED_INT,0);
    glUseProgram(0);
    std::cout << std::endl;
}

void Renderer::CleanUp(){
    glDeleteProgram(gGraphicsPipelineShaderProgram);
    glDeleteProgram(gGraphicsLighterPipelineShaderProgram);
}