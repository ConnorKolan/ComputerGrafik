#version 330 core

out vec4 FragColor;

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;


uniform sampler2D texture1;
uniform sampler2D texture2;

uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 lightPos;



void main()
{
   float ambientStength = 0.1;
   vec3 ambient = ambientStength * lightColor;
   
   vec3 norm = normalize(normal);
   vec3 lightDir = normalize(lightPos - fragPos);
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = diff * lightColor;

   float specularStrength = 0.5;
   vec3 viewDir = normalize(viewPos - fragPos);
   vec3 reflectDir = reflect(-lightDir, norm);  
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);
   vec3 specular = specularStrength * spec * lightColor;  


   vec3 result = (ambient + diffuse + specular);

   FragColor = texture(texture1, texCoord) * vec4(result, 1.0);
};