#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;

   float near = 1.0;
   float far = 100.0;

float LinearizeDepth(float depth) 
{

   float z = depth * 2.0 - 1.0; // back to NDC 
   return (2.0 * near * far) / (far + near - z * (far - near));	
}
void main()
{ 
    float depth = LinearizeDepth(texture(screenTexture, TexCoords).r) / far;
    FragColor = vec4(vec3(depth), 1.0);
}