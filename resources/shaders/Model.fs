#version 330 core
out vec4 FragColor;

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;
in vec4 lightSpaceCoords;

uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec4 color;
uniform bool hasTexture;

uniform sampler2D shadowMap;

uniform sampler2D texture0;
uniform float bias;

float shadowCalculation(vec4 lightSpacePos, sampler2D shadowMap){
   vec3 lightSpaceCoords = lightSpacePos.xyz / lightSpacePos.w;
   lightSpaceCoords = lightSpaceCoords * 0.5 + 0.5;
   float closestDepth = texture(shadowMap, lightSpaceCoords.xy).r; 
   float currentDepth = lightSpaceCoords.z;

   float shadow = 0.0;
   vec2 texelSize = 5.0 / textureSize(shadowMap, 0);
   for(int x = -1; x <= 1; ++x){
      for(int y = -1; y <= 1; ++y){
         float pcf = texture(shadowMap, lightSpaceCoords.xy + vec2(x, y) * texelSize).r; 
         shadow += currentDepth - bias > pcf  ? 1.0 : 0.0;        
      }    
   }

   shadow /= 15.0;
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
   
   return (ambient + (1.0 - shadowCalculation(lightSpaceCoords, shadowMap))  * (diffuse + specular));
}           



void main(){    
   if(!hasTexture){
      FragColor = color * vec4(lighting(), 1.0);
   }else{
      FragColor = texture(texture0, texCoord) * color * vec4(lighting() * 1.2, 1.0);
   }
}