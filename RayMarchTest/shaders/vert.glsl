#version 410 core
// From Vertex Buffer Object (VBO)
// The only thing that can come 'in', that is
// what our shader reads, the first part of the
// graphics pipeline.
layout(location=0) in vec3 position;
layout(location=1) in vec3 vertexColors;

// Uniform variables
uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_Projection; // for perspective

// Pass vertex colors into the fragment shader
out vec3 v_vertexColors;

void main()
{
  v_vertexColors = vertexColors;

  // TA_README: MVP matrix formula - you need to use these variables in this file in order to ensure main.cpp finds its locations
  mat4 mvpMatrix = u_Projection * u_ViewMatrix * u_ModelMatrix;

  vec4 newPosition =  mvpMatrix * vec4(position,1.0f);
  
  // TA_README:
	gl_Position = vec4(newPosition.x, newPosition.y, newPosition.z, newPosition.w);
}


