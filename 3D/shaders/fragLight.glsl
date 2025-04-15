#version 410 core

in vec3 v_vertexColors;
in vec3 v_vertexNormals;

//uniform vec3 objectColor;
//uniform vec3 lightColor;

out vec4 color;

// Entry point of program
void main()
{
	vec3 normals  = normalize(v_vertexNormals); 
	//color = vec4(normals.r,normals.g, normals.b, 1.0f);
	color = vec4(1.0f,1.0f, 1.0f, 1.0f);
}
