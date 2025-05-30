#version 410 core
// From Vertex Buffer Object (VBO)
// The only thing that can come 'in', that is
// what our shader reads, the first part of the
// graphics pipeline.
layout(location=0) in vec3 position;
layout(location=1) in vec3 vertexColors;
layout(location=2) in vec3 vertexNormals;

// Uniform variables
uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_Projection; // We'll use a perspective projection
uniform vec3 i_lightColor;
uniform vec3 i_lightPosition;
uniform vec3 i_viewPos;

// Pass vertex colors into the fragment shader
out vec3 v_vertexColors;
out vec3 v_vertexNormals;
out vec3 FragPos; 
out vec3 u_lightColor;
out vec3 u_lightPosition;
out vec3 u_viewPos;

void main()
{
  v_vertexColors = vertexColors;
  v_vertexNormals = vertexNormals;

  u_lightColor = i_lightColor;
  u_lightPosition = i_lightPosition;
  u_viewPos = i_viewPos;
  
  vec4 newPosition = u_Projection * u_ViewMatrix * u_ModelMatrix * vec4(position,1.0f);

  FragPos = vec3(u_ModelMatrix * vec4(position, 1.0));
  
  gl_Position = vec4(newPosition.x, newPosition.y, newPosition.z, newPosition.w);
}


