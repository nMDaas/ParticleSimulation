#include "Renderer.hpp"

Renderer::Renderer(){}

void Renderer::PreDraw(int gScreenWidth, int gScreenHeight) {
    glEnable(GL_DEPTH_TEST);                    
    glDisable(GL_CULL_FACE);

    // Initialize clear color
    // This is the background of the screen.
    glViewport(0, 0, gScreenWidth, gScreenHeight);
    glClearColor( 0.1f, 4.f, 7.f, 1.f );

    //Clear color buffer and Depth Buffer
  	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void Renderer::DrawLights(GLuint gGraphicsLighterPipelineShaderProgram, Particle gLightParticle, int gScreenWidth, int gScreenHeight, GLuint lightVertexArrayObject, GLuint lightVertexBufferObject, int gTotalIndices, Camera gCamera){
    PreDrawLight(gGraphicsLighterPipelineShaderProgram, gLightParticle, gScreenWidth, gScreenHeight);

    GLint u_ViewMatrixLocation = glGetUniformLocation(gGraphicsLighterPipelineShaderProgram,"u_ViewMatrix");
    if(u_ViewMatrixLocation>=0){
        glm::mat4 viewMatrix = gCamera.GetViewMatrix();
        glUniformMatrix4fv(u_ViewMatrixLocation,1,GL_FALSE,&viewMatrix[0][0]);
    }else{
        std::cout << "Could not find u_ModelMatrix, maybe a mispelling?\n";
        exit(EXIT_FAILURE);
    }

    DrawLight(lightVertexArrayObject, lightVertexBufferObject, gTotalIndices);
}

void Renderer::PreDrawLight(GLuint gGraphicsLighterPipelineShaderProgram, Particle gLightParticle, int gScreenWidth, int gScreenHeight) {
    // Use our shader
	glUseProgram(gGraphicsLighterPipelineShaderProgram);

    // Model transformation by translating our object into world space
    float r = gLightParticle.getRadius();
    glm::mat4 model = glm::translate(glm::mat4(1.0f), gLightParticle.getPosition());
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
                                             (float)gScreenWidth/(float)gScreenHeight,
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

void Renderer::DrawLight(GLuint lightVertexArrayObject, GLuint lightVertexBufferObject, int gTotalIndices){
    glBindVertexArray(lightVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, lightVertexBufferObject);
    glDrawElements(GL_TRIANGLES,gTotalIndices,GL_UNSIGNED_INT,0);
    glUseProgram(0);
}