#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 texCoord;
out vec3 normal;
out vec3 fragPos;
out vec4 lightSpaceCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;


void main(){
    texCoord = aTexCoord;
    fragPos = vec3(model * vec4(aPos, 1.0));
    lightSpaceCoords = lightSpaceMatrix * vec4(fragPos, 1.0);
    
    mat3 rotation = mat3(model[0].xyz, model[1].xyz, model[2].xyz);
    normal = vec3(rotation * aNormal);

    gl_Position = projection * view * model * vec4(aPos, 1.0);
};