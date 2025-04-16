#version 410 core

in vec3 v_vertexColors;
in vec3 v_vertexNormals;
in vec3 FragPos;  

out vec4 color;

// Entry point of program
void main()
{
    float ambient_strength = 0.1;
    float diffuse_strength = 0.9;

    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
    vec3 lightPosition = vec3(-3.0f,0.0f,0.0f);

    vec3 ambient = ambient_strength * lightColor;

	vec3 normals  = normalize(v_vertexNormals); 

    vec3 lightDir = normalize(lightPosition - FragPos);  
    float diff = max(dot(normals, lightDir), 0.0);
    vec3 diffuse = diffuse_strength * diff * lightColor;

    vec3 lightColor_x_objectColor = (ambient + diffuse) * vec3(normals.r, normals.g, normals.b);
	color = vec4(lightColor_x_objectColor, 1.0f);
}
