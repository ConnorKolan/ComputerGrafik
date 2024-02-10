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

        float velocity;

        Piece(Model* model, glm::mat4 transMatrix, glm::mat4 view, glm::mat4 projection){
            this->model = model;
            this->transMatrix = transMatrix;
            this->view = view;
            this->projection = projection;
        }

        void draw(Shader &shader){
            shader.setMat4("model", this->transMatrix);
            model->draw(shader);
        }

        void move(glm::vec3 movementVector){
            transMatrix = glm::translate(transMatrix, movementVector);
        }



};


#endif