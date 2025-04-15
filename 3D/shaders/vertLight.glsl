#version 410 core
layout(location=0) in vec3 position;

// Uniform variables
uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_Projection;

void main()
{
  gl_Position = u_Projection * u_ViewMatrix * u_ModelMatrix * vec4(position,1.0f);
}


