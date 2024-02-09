#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;
in vec3 fragPos;
in vec4 fragPosLightSpace;

uniform sampler2D screenTexture;

void main(){ 

    vec3 projCoords = fragPosLightSpace.xyz;
    projCoords = projCoords * 0.5 + 0.5;

    FragColor = texture(screenTexture, TexCoords);
}