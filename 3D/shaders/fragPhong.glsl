#version 410 core

in vec3 v_vertexColors;
in vec3 v_vertexNormals;
in vec3 FragPos;  

in vec3 u_lightColor;
in vec3 u_lightPosition;
in vec3 u_viewPos;

out vec4 color;

// Entry point of program
void main()
{
    float ambient_strength = 0.1;
    float diffuse_strength = 0.4;
    float specular_strength = 0.5;
    float shininess = 32;

    // Calculations for ambient lighting
    vec3 ambient = ambient_strength * u_lightColor;

    // Calculations for diffuse lighting 
	vec3 normals  = normalize(v_vertexNormals); // Currently important to visualize normals too
    vec3 lightDir = normalize(u_lightPosition - FragPos);  
    float diff = max(dot(normals, lightDir), 0.0);
    vec3 diffuse = diffuse_strength * diff * u_lightColor;

    // Calculations for specular lighting
    vec3 viewDir = normalize(u_viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normals);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specular_strength * spec * u_lightColor;  

    vec3 lightColor_x_objectColor = (ambient + diffuse + specular) * vec3(normals.r, normals.g, normals.b);
	color = vec4(lightColor_x_objectColor, 1.0f);
}
