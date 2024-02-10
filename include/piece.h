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

        void move(float value){
            this->transMatrix = glm::translate(this->transMatrix, value * this->getDirection());
        }

        glm::vec3 getDirection(){
            glm::vec4 direction4D = glm::vec4(direction, 1.0) * transMatrix;
            return glm::normalize(glm::vec3(direction4D.x, direction4D.y, direction4D.z));
        }

        glm::vec3 getPosition(){
            float x = this->transMatrix[3][0];
            float z = this->transMatrix[3][2];
            float y = this->transMatrix[3][1];

            return glm::vec3(x, y, z);
        }

};


#endif