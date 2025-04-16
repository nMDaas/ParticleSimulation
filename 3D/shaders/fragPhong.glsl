#version 410 core

in vec3 v_vertexColors;
in vec3 v_vertexNormals;
in vec3 FragPos;  

in vec3 u_lightColor;
in vec3 u_lightPosition;

out vec4 color;

// Entry point of program
void main()
{
    float ambient_strength = 0.1;
    float diffuse_strength = 0.9;

    vec3 ambient = ambient_strength * u_lightColor;

	vec3 normals  = normalize(v_vertexNormals); 

    vec3 lightDir = normalize(u_lightPosition - FragPos);  
    float diff = max(dot(normals, lightDir), 0.0);
    vec3 diffuse = diffuse_strength * diff * u_lightColor;

    vec3 lightColor_x_objectColor = (ambient + diffuse) * vec3(normals.r, normals.g, normals.b);
	color = vec4(lightColor_x_objectColor, 1.0f);
}
