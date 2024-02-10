#version 330 core
out vec4 FragColor;

in vec2 texCoord;
in vec3 normal;

void main(){    
   FragColor = texture(boardTexture, texCoord);
}