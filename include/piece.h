#ifndef PIECE
#define PIECE

#include <glm/glm.hpp>
#include <vector>

#include "shader.h"
#include <string>

class Piece{
    public:
        Model* model;
        glm::mat4 transMatrix;
        glm::mat4 view;
        glm::mat4 projection;
        glm::vec3 direction;


        float velocity;

        Piece(Model* model, glm::mat4 transMatrix, glm::mat4 view, glm::mat4 projection, glm::vec3 direction){
            this->model = model;
            this->transMatrix = transMatrix;
            this->view = view;
            this->projection = projection;
            this->direction = direction;
        }

        void draw(Shader &shader){
            shader.setMat4("model", this->transMatrix);
            model->draw(shader);
        }

        void move(float value, glm::vec3 direction){
            this->transMatrix = glm::translate(this->transMatrix, value * direction);
        }
};


#endif