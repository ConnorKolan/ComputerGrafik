#version 330 core
out vec4 FragColor;

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;
in vec4 fragPosLightSpace;

uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec4 color;
uniform sampler2D shadowMap;

float shadowCalculation()
{
   // perform perspective divide
   vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
   // transform to [0,1] range
   projCoords = projCoords * 0.5 + 0.5;
   // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
   float closestDepth = texture(shadowMap, projCoords.xy).r; 
   // get depth of current fragment from light's perspective
   float currentDepth = projCoords.z;
   // check whether current frag pos is in shadow
   float bias = 0.001;
   float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

   return shadow;
}  

vec3 lighting(){
   float ambientStength = 0.1;
   vec3 ambient = ambientStength * lightColor;
   
   vec3 norm = normalize(normal);
   vec3 lightDir = normalize(lightPos - fragPos);
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = diff * lightColor;

   float specularStrength = 0.5;
   vec3 viewDir = normalize(viewPos - fragPos);
   vec3 reflectDir = reflect(-lightDir, norm);  
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
   vec3 specular = specularStrength * spec * lightColor;  
   
   return  (ambient + (1.0 - shadowCalculation()) * (diffuse + specular));
}



void main(){    
   vec3 result = lighting();

   FragColor = color * vec4(lighting(), 1.0);
}