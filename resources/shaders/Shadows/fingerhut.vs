#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;


out vec3 fragPos;
out vec4 fragPosLightSpace;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main(){
    fragPos = vec3(model * vec4(aPos, 1.0));
    fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);

    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}