#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 texCoord;
out vec3 normal;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    texCoord = aTexCoord;
    fragPos = vec3(model * vec4(aPos, 1.0));

    //wir können es nicht einfach mit der model-Matrix multiplizieren, weil da ja auch andere transformationen als rotation drin sein können
    mat3 rotation  = mat3(model[0].xyz, model[1].xyz, model[2].xyz);
    normal = vec3(rotation * aNormal);

    gl_Position = projection * view * model * vec4(aPos, 1.0);
};