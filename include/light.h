#ifndef CUSTOMLIGHT
#define CUSTOMLIGHT

#include <glm/glm.hpp>
#include <camera.h>

class Light{
    public:
        glm::vec3 position;
        glm::vec3 color;
        Camera camera;

        Light(glm::vec3 position, glm::vec3 color, float yaw, float pitch){
            this->position = position;
            this->color = color;
            Camera cam(position, glm::vec3(0.0f, 1.0f, 0.0f), yaw, pitch);
            this->camera = cam;
        }
};

#endif