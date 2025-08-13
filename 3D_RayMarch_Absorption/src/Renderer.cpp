#include "Renderer.hpp"

Renderer::Renderer(){}

Renderer::Renderer(int i_screenWidth, int i_screenHeight, Scene* scene){
    screenWidth = i_screenWidth;
    screenHeight = i_screenHeight;
    mainScene = scene;

    rotZ = 0.0f;
}

void Renderer::updateZ(float val){
    rotZ = rotZ + val;
}

void Renderer::CreateGraphicsPipelines(){

    std::string vertexShaderSource      = LoadShaderAsString("./shaders/vertPhong.glsl");
    std::string fragmentShaderSource    = LoadShaderAsString("./shaders/fragPhong.glsl");

	gGraphicsPipelineShaderProgram = CreateShaderProgram(vertexShaderSource,fragmentShaderSource);


    std::string vertexShaderSource_light      = LoadShaderAsString("./shaders/vertLight.glsl");
    std::string fragmentShaderSource_light    = LoadShaderAsString("./shaders/fragLight.glsl");

    gGraphicsLighterPipelineShaderProgram = CreateShaderProgram(vertexShaderSource_light,fragmentShaderSource_light);

    std::string vertexShaderSource_rayMarch      = LoadShaderAsString("./shaders/vertRayMarch.glsl");
    std::string fragmentShaderSource_rayMarch     = LoadShaderAsString("./shaders/fragRayMarch.glsl");

    gGraphicsRayMarchingPipelineShaderProgram = CreateShaderProgram(vertexShaderSource_rayMarch,fragmentShaderSource_rayMarch);
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

void Renderer::RenderScene() {
    mainScene->InitializeGLuints();
    PreDraw();
    DrawParticles(mainScene->getObjTotalIndices("Particle"));
    DrawLights(mainScene->getObjTotalIndices("Light"));
    DrawBox(mainScene->getObjTotalIndices("Box"));
}

void Renderer::PreDraw() {
    glEnable(GL_DEPTH_TEST);                    
    glDisable(GL_CULL_FACE);

    // Initialize clear color
    // This is the background of the screen.
    glViewport(0, 0, screenWidth, screenHeight);
    glClearColor( 0.0f, 0.0f, 0.0f, 1.f );

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
}

void Renderer::DrawParticle(int i, int gTotalIndices){
    glBindVertexArray(mainScene->getGVertexArrayObjects_map()["Particle"][i]);
    glBindBuffer(GL_ARRAY_BUFFER, mainScene->getGVertexBufferObjects_map()["Particle"][i]);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 
    glDrawElements(GL_TRIANGLES,gTotalIndices,GL_UNSIGNED_INT,0);
    glUseProgram(0);
}

void Renderer::DrawLights(int gTotalIndices){
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
    GLuint lightVertexArrayObject = mainScene->getGVertexArrayObjects_map()["Light"][0];
    GLuint lightVertexBufferObject = mainScene->getGVertexBufferObjects_map()["Light"][0];
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 
    glBindVertexArray(lightVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, lightVertexBufferObject);
    glDrawElements(GL_TRIANGLES,gTotalIndices,GL_UNSIGNED_INT,0);
    glUseProgram(0);
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
}

void Renderer::PreDrawBox(){
    // Use our shader
	glUseProgram(gGraphicsLighterPipelineShaderProgram);

    // Model transformation by translating our object into world space
    float r = 0.2f;
    glm::mat4 model = mainScene->getBox()->getTransform();

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
}

void Renderer::DrawBoxActually(int gBoxTotalIndices){
    GLuint boxVertexArrayObject = mainScene->getGVertexArrayObjects_map()["Box"][0];
    GLuint boxVertexBufferObject = mainScene->getGVertexArrayObjects_map()["Box"][0];

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
    glBindVertexArray(boxVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, boxVertexArrayObject);
    glDrawElements(GL_TRIANGLES,gBoxTotalIndices,GL_UNSIGNED_INT,0);
    glUseProgram(0);
}

void Renderer::CleanUp(){
    glDeleteProgram(gGraphicsPipelineShaderProgram);
    glDeleteProgram(gGraphicsLighterPipelineShaderProgram);
    glDeleteProgram(gGraphicsRayMarchingPipelineShaderProgram);
}

void Renderer::VertexSpecification(){

	const std::vector<GLfloat> vertexData
	{
        -1.0f, -1.0f,
        3.0f, -1.0f,
        -1.0f,  3.0f
	};

	glGenVertexArrays(1, &gVertexArrayObject);
	glBindVertexArray(gVertexArrayObject);
	glGenBuffers(1, &gVertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
	
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

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*2, (void*)0);


	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void Renderer::RenderScene_RayMarch(){
    mainScene->InitializeGLuints();

    PreDraw();

    PreDraw_RM();
    Draw_RM(); // Draw raymarched particles on top

    DrawBox(mainScene->getObjTotalIndices("Box")); // Draw the box first

}


// NATASHA TODO! Something is weird with camera
//  Maybe perspec5ive and everything needs to be applied to positions before sending to shader
void Renderer::PreDraw_RM(){
    // Use our shader
	glUseProgram(gGraphicsRayMarchingPipelineShaderProgram);

    GLint locResolution = glGetUniformLocation(gGraphicsRayMarchingPipelineShaderProgram, "iResolution");
    glUniform2f(locResolution, (float)screenWidth, (float)screenHeight);

    GLint locCamPos = glGetUniformLocation(gGraphicsRayMarchingPipelineShaderProgram, "cameraPosition");
    glm::vec3 camPos = mainScene->getCamera()->GetCameraEyePosition();
    glUniform3fv(locCamPos, 1, &camPos[0]);

    GLint locCamRot = glGetUniformLocation(gGraphicsRayMarchingPipelineShaderProgram, "cameraRotation");
    glm::vec3 forward = glm::normalize(glm::vec3(0.0f,0.0f, -1.0f));
    glm::vec3 right   = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::vec3 up      = glm::cross(right, forward);
    glm::mat3 cameraRotation(right, up, -forward);
    glUniformMatrix3fv(locCamRot, 1, GL_FALSE, &cameraRotation[0][0]);

    float time = SDL_GetTicks() / 1000.0f;
    glUniform1f(glGetUniformLocation(gGraphicsRayMarchingPipelineShaderProgram, "iTime"), time);

    // Send shader particle info
    int numParticles = mainScene->getSolver()->getParticles().size();
    std::vector<glm::vec3> positions;
    for (int i = 0; i < numParticles; i++) {
        positions.push_back(mainScene->getSolver()->getParticles()[i]->getPosition());
    }

    glUniform1i(glGetUniformLocation(gGraphicsRayMarchingPipelineShaderProgram, "particleCount"), numParticles);
    glUniform3fv(glGetUniformLocation(gGraphicsRayMarchingPipelineShaderProgram, "particlePositions"), numParticles, &positions[0].x);
}

void Renderer::Draw_RM(){
    // Enable our attributes
    glBindVertexArray(gVertexArrayObject);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    // Select the vertex buffer object we want to enable
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);

    //Render data
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Stop using our current graphics pipeline
    // Note: This is not necessary if we only have one graphics pipeline.
    glUseProgram(0);
}