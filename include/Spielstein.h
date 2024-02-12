#include <glm/glm.hpp>
#include <shader.h>
#include <model.h>


class Spielstein{

    public:

        glm::mat4 translationMatrix;
        glm::mat4 viewMat;
        glm::mat4 projectionMat;
        Model* someInformation;

        Spielstein(glm::mat4 matrix, glm::mat4 view, glm::mat4 projection, Model* model){
            this->translationMatrix=matrix;
            this->viewMat=view;
            this->projectionMat=projection;
            this->someInformation= model;
        }

        void incX(){
            this->translationMatrix = glm::translate(this->translationMatrix, glm::vec3(0.01f,.0f,.0f));
        }

        void decX(){
            this->translationMatrix= glm::translate(this->translationMatrix, glm::vec3(-0.01f,.0f,.0f));
        }

        void incY(){
            this->translationMatrix= glm::translate(this->translationMatrix, glm::vec3(.0f,0.01f,.0f));
        }
        
        void decY(){
            this->translationMatrix= glm::translate(this->translationMatrix, glm::vec3(.0f,-0.01f,.0f));
        }

        void draw(Shader& shader){
            shader.use();
            shader.setMat4("model", translationMatrix);
            shader.setMat4("view", viewMat);
            shader.setMat4("projection", projectionMat);

            someInformation->draw(shader);
        }

};